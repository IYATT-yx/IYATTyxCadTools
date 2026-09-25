/**
 * @file      UtilEntityStyle.ixx
 * @brief     实体样式模块。
 * @author    IYATT-yx
 * @copyright Copyright (c) 2026 IYATT-yx.
 *            Licensed under the MIT License. See LICENSE file in the project root for full license information.
 */
module;
#include "StdAfx.h"

export module UtilEntityStyle;

export namespace UtilEntityStyle
{
    void setByLayer(const AcDbObjectId& objId);
}