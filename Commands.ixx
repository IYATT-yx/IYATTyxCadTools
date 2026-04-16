module;
#include "stdafx.h"

export module Commands;

export namespace Commands
{
	enum CommandFlags : int
	{
		Base = ACRX_CMD_MODAL, // 模态命令
		//Pick = ACRX_CMD_MODAL | ACRX_CMD_USEPICKSET, // 模态命令，支持预选（先选实体后执行命令）
		PickRedraw = ACRX_CMD_MODAL | ACRX_CMD_USEPICKSET | ACRX_CMD_REDRAW // 模态命令，支持预选，支持重绘
	};

	struct CommandInfo
	{
		AcString commandName; // 命令全名
		AcString commandDescription; // 命令描述
		Commands::CommandFlags flags; // 命令标志
		AcRxFunctionPtr proc; // 命令处理函数

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

	using CommandInfoList = std::vector<Commands::CommandInfo>;
	CommandInfoList commandInfoList;

	using CommandList = std::vector<const wchar_t*>;
};

export namespace Commands
{
	/**
	 * @brief 注册命令
	 * @param cil 命令列表
	 */
	void registerYxCmds(Commands::CommandInfoList& cil);

	/**
	 * @brief 执行命令，命令列表第一个元素自动加上“._” 开头
	 * @param cmdList 命令列表
	 * @param pDoc 文档指针
	 */
	void executeCommand(const Commands::CommandList& cmdList, AcApDocument* pDoc = nullptr);
};