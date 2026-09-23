/**
 * @file      Interface.ixx
 * @brief     接口模块
 * @author    IYATT-yx
 * @copyright Copyright (c) 2026 IYATT-yx.
 *            Licensed under the MIT License. See LICENSE file in the project root for full license information.
 */
export module Interface;

export class Interface
{
public:
	static void init();
	static void unload();
	static void cmdYx();
private:
	static void info();
};

