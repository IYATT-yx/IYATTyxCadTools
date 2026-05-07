/**
 * @file      ConfigManager.ixx
 * @brief     配置管理模块，负责全局设置的持久化与内存同步。
 * @details   采用单例模式管理配置项，支持通过 nlohmann/json 实现结构体与 JSON 文件的自动映射。
 *            具备配置自愈能力：当物理文件缺失或字段缺失时，自动补全默认值并同步回磁盘。
 * @author    IYATT-yx
 * @copyright Copyright (c) 2026 IYATT-yx.
 *            Licensed under the MIT License. See LICENSE file in the project root for full license information.
 */
module;
#include "thirdparty/nlohmann/json.hpp"

export module ConfigManager;

// 设置项
export namespace ConfigItems
{
	// 输入法语言自动切换设置
	struct ImeSettings
	{
		bool bAutoStart = false;
		unsigned long iIntervalMs = 200;
		NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(ImeSettings, bAutoStart, iIntervalMs)
	};

	// 中键映射确定按钮设置
	struct MiddleClickManagerSettings
	{
		bool bAutoStart = false;
        NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(MiddleClickManagerSettings, bAutoStart)
	};

	// 全局配置
	struct GlobalConfig
	{
		ImeSettings imeSettings;
		MiddleClickManagerSettings middleClickManagerSettings;
		NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(GlobalConfig, imeSettings, middleClickManagerSettings)
	};
};

export class ConfigManager
{
public:
	/**
	* @brief 获取 ConfigManager 全局唯一实例。
	* @return ConfigManager 引用。
	*/
	static ConfigManager& getInstance();

	// 禁止拷贝和赋值操作
	ConfigManager(const ConfigManager&) = delete;
    ConfigManager& operator=(const ConfigManager&) = delete;

	ConfigItems::GlobalConfig& getConfig();

private:
	ConfigManager() = default;
	ConfigItems::GlobalConfig mConfig;
	std::wstring mConfigFilename;
	std::wstring mLastError;

public:
	/**
	* @brief 从指定文件加载配置数据。
	*
	* 该函数会尝试解析 JSON 文件并映射到内存结构体。若文件缺失，会触发
	* IYATTyxJsonWrapper 的自愈机制创建一个空的 JSON 对象。加载成功后，
	* 会强制执行一次 saveConfig() 以确保物理文件与内存结构体定义同步。
	*
	* @param filename 配置文件完整的宽字符路径。
	* @return true 加载及同步保存成功；false 过程出错（错误信息可通过 getLastError 获取）。
	*/
	bool loadConfig(const std::wstring& filename);

	/**
	* @brief 将当前内存配置持久化至磁盘。
	*
	* 使用 nlohmann/json 自动序列化技术将 mConfig 转换为 JSON。
	*
	* @return true 保存成功；false 发生序列化异常或 IO 错误。
	*/
	bool saveConfig();

	/**
	* @brief 获取最后一次发生的错误描述。
	* @return 宽字符串格式的错误信息。
	*/
	std::wstring ConfigManager::getLastError() const;

	/**
	* @brief 获取当前加载的配置文件全路径。
	* @return 存储配置文件的完整物理路径字符串；若尚未加载配置，则返回空字符串。
	*/
	std::wstring getConfigFilename();
};