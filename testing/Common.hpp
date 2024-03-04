#pragma once

#include <sqee/setup.hpp>

#include <sqee/core/TypeAliases.hpp>

#include <sqee/maths/Vectors.hpp>
#include <sqee/maths/Matrices.hpp>
#include <sqee/maths/Quaternion.hpp>

#include <ostream>

#include "catch_amalgamated.hpp"

//============================================================================//

using namespace sq::coretypes;

using Catch::Matchers::WithinRel;
using Catch::Matchers::MatcherBase;

namespace maths = sq::maths;

constexpr const float PI = 3.14159265358979323846f;

//============================================================================//

namespace sq::maths {

template <int S, class T>
inline std::ostream& operator<<(std::ostream& os, const Vector<S, T>& arg)
{ return os << fmt::to_string(arg); }

template <int H, int W, class T>
inline std::ostream& operator<<(std::ostream& os, const Matrix<H, W, T>& arg)
{ return os << fmt::to_string(arg); }

template <class T>
inline std::ostream& operator<<(std::ostream& os, const Quaternion<T>& arg)
{ return os << fmt::to_string(arg); }

} // namespace sq::maths

//============================================================================//

template <int S, class T>
class WithinAbsMatcher_Vector final : public MatcherBase<maths::Vector<S, T>>
{
public:
    WithinAbsMatcher_Vector(maths::Vector<S, T> target, double margin)
        : m_target(target), m_margin(margin) {}

    bool match(const maths::Vector<S, T>& matchee) const override
    {
        bool result = true;
        for (int i = 0; i < S; ++i)
            result &= Catch::Matchers::WithinAbs(m_target[i], m_margin).match(matchee[i]);
        return result;
    }

    std::string describe() const override
    {
        Catch::ReusableStringStream sstr;
        sstr << "is within " << m_margin << " of " << m_target;
        return sstr.str();
    }

private:
    maths::Vector<S, T> m_target;
    double m_margin;
};

template <int W, int H, class T>
class WithinAbsMatcher_Matrix final : public MatcherBase<maths::Matrix<W, H, T>>
{
public:
    WithinAbsMatcher_Matrix(maths::Matrix<W, H, T> target, double margin)
        : m_target(target), m_margin(margin) {}

    bool match(const maths::Matrix<W, H, T>& matchee) const override
    {
        bool result = true;
        for (int i = 0; i < W; ++i)
            for (int j = 0; j < H; ++j)
                result &= Catch::Matchers::WithinAbs(m_target[i][j], m_margin).match(matchee[i][j]);
        return true;
    }

    std::string describe() const override
    {
        Catch::ReusableStringStream sstr;
        sstr << "is within " << m_margin << " of " << m_target;
        return sstr.str();
    }

private:
    maths::Matrix<W, H, T> m_target;
    double m_margin;
};

template <class T>
class WithinAbsMatcher_Quaternion final : public MatcherBase<maths::Quaternion<T>>
{
public:
    WithinAbsMatcher_Quaternion(maths::Quaternion<T> target, double margin)
        : m_target(target), m_margin(margin) {}

    bool match(const maths::Quaternion<T>& matchee) const override
    {
        bool resultPos = true;
        for (int i = 0; i < 4; ++i)
            resultPos &= Catch::Matchers::WithinAbs(+m_target[i], m_margin).match(matchee[i]);
        bool resultNeg = true;
        for (int i = 0; i < 4; ++i)
            resultPos &= Catch::Matchers::WithinAbs(-m_target[i], m_margin).match(matchee[i]);
        return resultPos || resultNeg;
    }

    std::string describe() const override
    {
        Catch::ReusableStringStream sstr;
        sstr << "is within " << m_margin << " of " << m_target;
        return sstr.str();
    }

private:
    maths::Quaternion<T> m_target; T m_margin;
};

//============================================================================//

constexpr double DEFAULT_MARGIN = 0.000001;

template <std::floating_point T>
Catch::Matchers::WithinAbsMatcher WithinAbs(T target, double margin = DEFAULT_MARGIN)
{
    return Catch::Matchers::WithinAbsMatcher(target, margin);
}

template <int S, class T>
WithinAbsMatcher_Vector<S, T> WithinAbs(maths::Vector<S, T> target, double margin = DEFAULT_MARGIN)
{
    return WithinAbsMatcher_Vector(target, margin);
}

template <int W, int H, class T>
WithinAbsMatcher_Matrix<W, H, T> WithinAbs(maths::Matrix<W, H, T> target, double margin = DEFAULT_MARGIN)
{
    return WithinAbsMatcher_Matrix(target, margin);
}

template <class T>
WithinAbsMatcher_Quaternion<T> WithinAbs(maths::Quaternion<T> target, double margin = DEFAULT_MARGIN)
{
    return WithinAbsMatcher_Quaternion(target, margin);
}
