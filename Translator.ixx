/**
 * @file      Translator.ixx
 * @brief     翻译器模块
 * @author    IYATT-yx
 * @copyright Copyright (c) 2026 IYATT-yx.
 *            Licensed under the MIT License. See LICENSE file in the project root for full license information.
 */
export module Translator;
import std;
import EncodingConverter;

export class Translator
{
public:
	/**
	 * @brief 获取翻译器实例
	 * @return 翻译器实例
	 */
	static Translator& getInstance();
	/**
	 * @brief 初始化翻译器
	 * @param localesPath 语言文件目录
	 * @param languageCode 语言代码
	 * @return true 初始化成功，false 初始化失败
	 */
	bool initialize(const std::filesystem::path& localesPath, const std::wstring& languageCode = L"zh_CN");
	/**
	 * @brief 翻译函数，用于获取翻译后的文本
	 * @param text 待翻译的文本
	 * @param domain 域名
	 * @return 翻译后的文本
	 */
	const wchar_t* translate(const wchar_t* text, const std::wstring& domain) const;

private:
	std::atomic<bool> mStatus = false;
	std::filesystem::path mLocalesPath;
	std::wstring mLanguageCode;

	mutable std::shared_mutex mTranslationLock;
	mutable std::unordered_map<std::wstring, std::wstring> mTranslationMap;

	// 🔒 【绝对安全的终生驻留池】
	// 利用 std::list 的物理不重定位特性，确保追加新字符串时，已有字符串的内存地址永不改变
	mutable std::list<std::wstring> mStringPool;
	// ⚡ 【高性能快查索引】
	// Key: 原文 (源语言), Value: 驻留池中译文的 wchar_t* 指针
	mutable std::unordered_map<std::wstring, const wchar_t*> mStringIndex;

	mutable std::mutex mDomainMutex;
	mutable std::unordered_set<std::wstring> mLoadedDomains;

	/**
	 * @brief 内部方法：纯内存解析 GNU gettext MO 二进制文件
	 * @param path MO 文件路径
	 * @param domain 域名
	 * @return 翻译字典
	 */
	std::unordered_map<std::wstring, std::wstring>loadMoFile(const std::filesystem::path& path, const std::wstring& domain) const;
};

/**
 * @brief 翻译函数，用于获取翻译后的文本
 * @param text 待翻译的文本
 * @param domain 域名
 * @return 翻译后的文本
 */
export inline auto _(const wchar_t* text, const wchar_t* domain = L"messages")
{
	return Translator::getInstance().translate(text, domain);
}

/**
 * @brief 宽字符串版字长
 */
export class WException : public std::exception
{
public:
	explicit WException(std::wstring msg)
		: m_msg(std::move(msg))
	{}

	const std::wstring& message() const noexcept
	{
		return m_msg;
	}

private:
	std::wstring m_msg;
};