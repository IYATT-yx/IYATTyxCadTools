/**
 * @file      IYATTyxJsonWrapper.ixx
 * @brief     nlohmann/json 库的模块化包装接口。
 * @author    IYATT-yx
 * @copyright Copyright (c) 2026 IYATT-yx.
 *            Licensed under the MIT License. See LICENSE file in the project root for full license information.
 */
module;
#include "thirdparty/nlohmann/json.hpp"

export module IYATTyxJsonWrapper;
import std;

export namespace IYATTyxJsonWrapper
{
	using json = nlohmann::json;
};

export namespace IYATTyxJsonWrapper
{
    /**
    * @brief 从指定路径打开并解析 JSON 文件。
    *
    * 该函数具备自动初始化功能：若目标文件不存在，将尝试创建一个内容为 "{}" 的空对象文件。
    * 解析过程中发生的任何标准异常或未知异常都会被捕获，并通过 errorMsg 返回描述。
    *
    * @param filename 文件的宽字符完整路径。
    * @param errorMsg [out] 用于接收执行过程中产生的错误描述（若无错误则为空字符串）。
    * @return 解析后的 json 对象。若文件打开失败或解析异常，返回一个空的 json 对象 (json::object())。
    */
	IYATTyxJsonWrapper::json openJson(const std::wstring& filename, std::wstring& errorMsg);

    /**
    * @brief 将 JSON 对象持久化到磁盘。
    *
    * 该函数执行全覆盖写入，并使用 4 空格缩进进行格式化。
    * 写入前后均会进行严格的流状态检查，并捕获文件系统相关的异常。
    *
    * @param filename 目标文件的宽字符完整路径。
    * @param json     待保存的 json 对象。
    * @param errorMsg [out] 用于接收执行过程中产生的错误描述。
    * @return true 如果保存成功；false 如果流无法开启或写入过程抛出异常。
    */
	bool saveJson(const std::wstring& filename, const IYATTyxJsonWrapper::json& json, std::wstring& errorMsg);
};