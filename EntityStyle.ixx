/**
 * @file      EntityStyle.ixx
 * @brief     实体样式模块。
 * @author    IYATT-yx
 * @copyright Copyright (c) 2026 IYATT-yx.
 *            Licensed under the MIT License. See LICENSE file in the project root for full license information.
 */
module;
#include "StdAfx.h"

export module EntityStyle;

export namespace EntityStyle
{
    void setByLayer(const AcDbObjectId& objId);
}