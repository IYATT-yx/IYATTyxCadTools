/**
 * @file      AcadVarUtil.ixx
 * @brief     系统变量模块，负责 AutoCAD 环境变量的安全读取与原子化写入。
 * @author    IYATT-yx
 * @copyright Copyright (c) 2026 IYATT-yx.
 *            Licensed under the MIT License. See LICENSE file in the project root for full license information.
 */
module;
#include "StdAfx.h"

export module AcadVarUtil;

namespace AcadVar
{
    enum class VarType : short
    {
        None = RTNONE,
        Real = RTREAL,
        Point2d = RTPOINT,
        Short = RTSHORT,
        Str = RTSTR,
        Entity = RTENAME,
        Point3d = RT3DPOINT,
        Long = RTLONG
    };

    template <typename T>
    struct TypeMapping;

    template <>
    struct TypeMapping<double>
    {
        static constexpr VarType value = VarType::Real;
    };

    template <>
    struct TypeMapping<short>
    {
        static constexpr VarType value = VarType::Short;
    };

    template <>
    struct TypeMapping<int>
    {
        static constexpr VarType value = VarType::Short;
    };

    template <>
    struct TypeMapping<long>
    {
        static constexpr VarType value = VarType::Long;
    };

    template <>
    struct TypeMapping<AcString>
    {
        static constexpr VarType value = VarType::Str;
    };

    template <>
    struct TypeMapping<ads_point>
    {
        static constexpr VarType value = VarType::Point3d;
    };

    template <>
    struct TypeMapping<ads_name>
    {
        static constexpr VarType value = VarType::Entity;
    };
}

export namespace AcadVarName
{
    constexpr const wchar_t* TEXTSIZE = L"TEXTSIZE";
};

export class AcadVarUtil
{
public:
    /**
     * @brief 通用获取 AutoCAD 系统变量。
     * * @tparam T 目标数据类型。支持 double, int, short, long, AcString, ads_point, ads_name。
     * @param[in] pszName 系统变量名称（必须以 L 开头，如 L"TEXTSIZE"）。
     * @param[out] outVal 获取到的变量值。
     * @return true 获取成功且类型匹配。
     * @return false 获取失败或返回类型与预期不符。
     */
    template <typename T>
    static bool getVar(const wchar_t* pszName, T& outVal)
    {
        constexpr AcadVar::VarType expected = AcadVar::TypeMapping<T>::value;
        resbuf rb;

        if (acedGetVar(pszName, &rb) == RTNORM)
        {
            // 校验底层类型是否符合预期
            if (static_cast<short>(expected) != rb.restype)
            {
                // 字符串特殊处理：释放缓冲区
                if (rb.restype == RTSTR && rb.resval.rstring != nullptr)
                {
                    acutDelString(rb.resval.rstring);
                }
                return false;
            }

            // 根据类型分发到对应的联合体成员 (resval.xxx)
            if constexpr (expected == AcadVar::VarType::Real)
            {
                outVal = rb.resval.rreal;
            }
            else if constexpr (expected == AcadVar::VarType::Short)
            {
                outVal = static_cast<T>(rb.resval.rint);
            }
            else if constexpr (expected == AcadVar::VarType::Str)
            {
                outVal = rb.resval.rstring;
                if (rb.resval.rstring != nullptr)
                {
                    acutDelString(rb.resval.rstring);
                }
            }
            else if constexpr (expected == AcadVar::VarType::Point3d || expected == AcadVar::VarType::Point2d)
            {
                outVal[X] = rb.resval.rpoint[X];
                outVal[Y] = rb.resval.rpoint[Y];
                outVal[Z] = rb.resval.rpoint[Z];
            }
            else if constexpr (expected == AcadVar::VarType::Long)
            {
                outVal = rb.resval.rlong;
            }
            else if constexpr (expected == AcadVar::VarType::Entity)
            {
                outVal[0] = rb.resval.rlname[0];
                outVal[1] = rb.resval.rlname[1];
            }

            return true;
        }

        return false;
    }

    /**
     * @brief 通用设置 AutoCAD 系统变量。
     * * @tparam T 传入数据类型。支持 double, int, short, long, AcString, const wchar_t*, ads_point, ads_name。
     * @param[in] pszName 系统变量名称（如 L"OSMODE"）。
     * @param[in] val 待设置的新值。
     * @return true 设置成功。
     * @return false 设置失败（可能变量名不存在或变量为只读）。
     */
    template <typename T>
    static bool setVar(const wchar_t* pszName, const T& val)
    {
        constexpr AcadVar::VarType type = AcadVar::TypeMapping<T>::value;
        resbuf rb;
        rb.restype = static_cast<short>(type);
        rb.rbnext = nullptr;

        if constexpr (type == AcadVar::VarType::Real)
        {
            rb.resval.rreal = val;
        }
        else if constexpr (type == AcadVar::VarType::Short)
        {
            rb.resval.rint = static_cast<short>(val);
        }
        else if constexpr (type == AcadVar::VarType::Str)
        {
            rb.resval.rstring = const_cast<wchar_t*>(static_cast<const wchar_t*>(val));
        }
        else if constexpr (type == AcadVar::VarType::Point3d || type == AcadVar::VarType::Point2d)
        {
            rb.resval.rpoint[X] = val[X];
            rb.resval.rpoint[Y] = val[Y];
            rb.resval.rpoint[Z] = val[Z];
        }
        else if constexpr (type == AcadVar::VarType::Long)
        {
            rb.resval.rlong = val;
        }
        else if constexpr (type == AcadVar::VarType::Entity)
        {
            rb.resval.rlname[0] = val[0];
            rb.resval.rlname[1] = val[1];
        }

        if (acedSetVar(pszName, &rb) == RTNORM)
        {
            return true;
        }

        return false;
    }
};