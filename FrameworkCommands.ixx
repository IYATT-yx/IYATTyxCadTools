/**
 * @file      FrameworkCommands.ixx
 * @brief     命令模块接口
 * @author    IYATT-yx
 * @copyright Copyright (c) 2026 IYATT-yx.
 *            Licensed under the MIT License. See LICENSE file in the project root for full license information.
 */
module;
#include "StdAfx.h"

export module FrameworkCommands;
import std;

export namespace FrameworkCommands
{
	enum CommandFlags : int
	{
		Base = ACRX_CMD_MODAL,                                                // 模态命令
		PickRedraw = ACRX_CMD_MODAL | ACRX_CMD_USEPICKSET | ACRX_CMD_REDRAW   // 模态命令，支持预选与重绘
	};

	struct CommandInfo
	{
		AcString commandName;         // 命令全名
		AcString commandDescription;  // 命令描述
		FrameworkCommands::CommandFlags flags; // 命令标志
		AcRxFunctionPtr proc;         // 命令处理函数

		/**
		 * @brief 获取命令的简名
		 * @return 命令简名。yx 开头，加命令全名中的大写字母拼接。
		 */
		AcString getShortCommandName() const
		{
			AcString prefix = L"yx";

			if (this->commandName.isEmpty())
			{
				return prefix;
			}

			AcString shortCommandName;
			for (size_t i = 0; i < this->commandName.length(); ++i)
			{
				wchar_t ch = this->commandName[i];
				if (iswupper(ch))
				{
					shortCommandName += ch;
				}
			}
			return prefix + shortCommandName;
		}
	};

	using CommandInfoList = std::vector<FrameworkCommands::CommandInfo>;
	inline CommandInfoList commandInfoList; // 全局命令列表缓存

	using CommandList = std::vector<const wchar_t*>;

	/**
	 * @brief 静态注册用的描述信息结构体
	 */
	struct CommandSpec
	{
		AcString commandName;
		std::function<AcString()> getLocalizedDescription;
		FrameworkCommands::CommandFlags flags;
		AcRxFunctionPtr proc;
	};

	/**
	 * @brief 命令自注册中心单例
	 */
	class CommandRegistry
	{
	public:
		static CommandRegistry& getInstance()
		{
			static CommandRegistry instance;
			return instance;
		}

		void addSpec(CommandSpec spec)
		{
			this->specs.push_back(std::move(spec));
		}

		const std::vector<CommandSpec>& getSpecs() const
		{
			return this->specs;
		}

	private:
		CommandRegistry() = default;
		std::vector<CommandSpec> specs;
	};

	struct AutoRegister
	{
		// 单个注册
		AutoRegister(CommandSpec spec)
		{
			CommandRegistry::getInstance().addSpec(std::move(spec));
		}

		// 批量注册
		AutoRegister(std::initializer_list<CommandSpec> specs)
		{
			auto& reg = CommandRegistry::getInstance();
			for (const auto& spec : specs)
			{
				reg.addSpec(spec);
			}
		}
	};

	/**
	 * @brief 收集已自注册的命令并向 CAD 系统注册
	 */
	void registerYxCmds();

	/**
	 * @brief 执行命令
     * @param cmdList 命令列表
	 * @param usePrefix 是否使用 ._ 前缀
	 * @param pDoc 文档指针
	 */
	void executeCommand(const FrameworkCommands::CommandList& cmdList, bool usePrefix = true, AcApDocument* pDoc = nullptr);
};