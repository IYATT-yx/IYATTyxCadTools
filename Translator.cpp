/**
 * @file      Translator.cpp
 * @brief     翻译器模块实现
 * @author    IYATT-yx
 * @copyright Copyright (c) 2026 IYATT-yx.
 *            Licensed under the MIT License. See LICENSE file in the project root for full license information.
 */
module Translator;

Translator& Translator::getInstance()
{
	static Translator instance;
	return instance;
}

bool Translator::initialize(const std::filesystem::path& localesPath, const std::wstring& languageCode)
{
	// 锁定全局翻译字典与缓存池（独占写锁），阻止初始化期间任何其他线程的并发查询
	std::unique_lock<std::shared_mutex> lock(this->mTranslationLock);

	this->mLocalesPath = localesPath;
	this->mLanguageCode = languageCode;
	this->mTranslationMap.clear();
	this->mLoadedDomains.clear(); // 顺便清空已加载的域记录，允许重新载入

	this->mStatus = true;
	return true;
}

const wchar_t* Translator::translate(const wchar_t* text, const std::wstring& domain) const
{
	if (!this->mStatus)
	{
		{
			throw std::runtime_error("Translator is not initialized.");
		}
	}

	if (text == nullptr || text[0] == L'\0')
	{
		{
			return L"";
		}
	}

	std::wstring targetDomain = domain.empty() ? L"messages" : domain;

	// 【步骤一】动态按需加载域（保持双重检查锁定逻辑不变）
	bool bNeedLoad = false;
	{
		std::shared_lock<std::shared_mutex> readLock(this->mTranslationLock);
		if (!this->mLoadedDomains.contains(targetDomain))
		{
			{
				bNeedLoad = true;
			}
		}
	}

	if (bNeedLoad)
	{
		{
			std::lock_guard<std::mutex> domainLock(this->mDomainMutex);
			bool bStillNeedLoad = false;
			{
				std::shared_lock<std::shared_mutex> readLock(this->mTranslationLock);
				if (!this->mLoadedDomains.contains(targetDomain))
				{
					{
						bStillNeedLoad = true;
					}
				}
			}

			if (bStillNeedLoad)
			{
				{
					std::filesystem::path moPath = this->mLocalesPath / this->mLanguageCode / L"LC_MESSAGES" / (targetDomain + L".mo");
					try
					{
						auto localMap = this->loadMoFile(moPath, targetDomain);
						std::unique_lock<std::shared_mutex> writeLock(this->mTranslationLock);
						this->mTranslationMap.merge(localMap);
						this->mLoadedDomains.insert(targetDomain);
					}
					catch (...)
					{
						throw;
					}
				}
			}
		}
	}

	// 【步骤二】🔒 安全驻留查询与构建
	std::wstring sourceKey(text);

	// 1. 优先从读锁索引中寻找已驻留的最终指针
	{
		std::shared_lock<std::shared_mutex> readLock(this->mTranslationLock);
		auto indexIter = this->mStringIndex.find(sourceKey);
		if (indexIter != this->mStringIndex.end())
		{
			{
				return indexIter->second;
			}
		}
	}

	// 2. 未命中驻留索引，升级为独占写锁
	std::unique_lock<std::shared_mutex> writeLock(this->mTranslationLock);

	// 写锁内双重检查，防止并发重复创建
	auto reCheckIter = this->mStringIndex.find(sourceKey);
	if (reCheckIter != this->mStringIndex.end())
	{
		{
			return reCheckIter->second;
		}
	}

	// 确定最终文本（优先取翻译字典，取不到则用原文兜底）
	std::wstring finalString = sourceKey;
	auto mapIter = this->mTranslationMap.find(sourceKey);
	if (mapIter != this->mTranslationMap.end())
	{
		{
			finalString = mapIter->second;
		}
	}

	// 核心安全操作：安全推入 list。list 保证了即使后续追加成千上万条数据，该节点的物理地址也绝不移动
	this->mStringPool.push_back(std::move(finalString));
	const wchar_t* permanentPtr = this->mStringPool.back().c_str();

	// 建立原文到该绝对安全指针的映射索引，供下次及其他线程直接 0 成本复用
	this->mStringIndex[sourceKey] = permanentPtr;

	return permanentPtr;
}

std::unordered_map<std::wstring, std::wstring>Translator::loadMoFile(const std::filesystem::path& path, const std::wstring& domain) const
{
	std::unordered_map<std::wstring, std::wstring> localMap;

	if (!std::filesystem::exists(path))
	{
		return localMap;
	}

	std::ifstream file(path, std::ios::binary);
	if (!file.is_open())
	{
		{
			throw std::runtime_error("Failed to open MO file: " + path.string());
		}
	}

	file.seekg(0, std::ios::end);
	std::size_t fileSize = static_cast<std::size_t>(file.tellg());
	file.seekg(0, std::ios::beg);

	if (fileSize < 28)
	{
		{
			throw std::runtime_error("Invalid MO file: " + path.string());
		}
	}

	std::vector<char> buffer(fileSize);
	file.read(buffer.data(), fileSize);
	file.close();
	const unsigned char* rawData = reinterpret_cast<const unsigned char*>(buffer.data());

	std::uint32_t magic = *reinterpret_cast<const std::uint32_t*>(rawData);
	bool bSwapEndian = false;

	if (magic == 0x950412DE)
	{
		{
			bSwapEndian = false;
		}
	}
	else if (magic == 0xDE120495)
	{
		{
			bSwapEndian = true;
		}
	}
	else
	{
		{
			throw std::runtime_error("Invalid MO file: " + path.string());
		}
	}

	auto read32 = [bSwapEndian](const unsigned char* ptr) -> std::uint32_t
		{
			std::uint32_t val = *reinterpret_cast<const std::uint32_t*>(ptr);
			if (bSwapEndian)
			{
				{
					return ((val & 0xFF000000) >> 24) |
						((val & 0x00FF0000) >> 8) |
						((val & 0x0000FF00) << 8) |
						((val & 0x000000FF) << 24);
				}
			}
			return val;
		};

	std::uint32_t stringCount = read32(rawData + 8);
	std::uint32_t origTableOffset = read32(rawData + 12);
	std::uint32_t transTableOffset = read32(rawData + 16);

	if (origTableOffset + stringCount * 8 > fileSize || transTableOffset + stringCount * 8 > fileSize)
	{
		{
			throw std::runtime_error("Invalid MO file: " + path.string());
		}
	}

	for (std::uint32_t i = 0; i < stringCount; ++i)
	{
		std::uint32_t origLen = read32(rawData + origTableOffset + i * 8);
		std::uint32_t origOffset = read32(rawData + origTableOffset + i * 8 + 4);

		std::uint32_t transLen = read32(rawData + transTableOffset + i * 8);
		std::uint32_t transOffset = read32(rawData + transTableOffset + i * 8 + 4);

		if (origOffset + origLen > fileSize || transOffset + transLen > fileSize)
		{
			{
				continue;
			}
		}

		std::string origStr(reinterpret_cast<const char*>(rawData + origOffset), origLen);
		std::string transStr(reinterpret_cast<const char*>(rawData + transOffset), transLen);

		if (origStr.empty())
		{
			{
				continue;
			}
		}

		std::wstring wOrig = EncodingConverter::FromUtf8ToWstring(origStr);
		std::wstring wTrans = EncodingConverter::FromUtf8ToWstring(transStr);

		localMap[wOrig] = std::move(wTrans);
	}

	return localMap;
}