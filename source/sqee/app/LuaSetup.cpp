#include <sqee/app/LuaSetup.hpp>

#include <sqee/core/Types.hpp>

using namespace sq;

//============================================================================//

namespace { // anonymous

//----------------------------------------------------------------------------//

template <int S, class T> inline
void impl_setup_VectorST(sol::state& /*lua*/, const String& /*name*/)
{
//    using VecST = maths::Vector<S, T>;

//    m.add(constructor<VecST(const VecST&)>(), name);
//    m.add(fun(&VecST::operator=), "=");

//    m.add(vector_conversion<Vector<VecST>>());
//    m.add(type_conversion<VecST, String, String(*)(const VecST&)>(&chai_string));

//    m.add(fun<VecST, VecST, VecST>(maths::operator+), "+");
//    m.add(fun<VecST, VecST, VecST>(maths::operator-), "-");
//    m.add(fun<VecST, VecST, VecST>(maths::operator*), "*");
//    m.add(fun<VecST, VecST, VecST>(maths::operator/), "/");

//    m.add(fun<VecST, VecST, T>(maths::operator+), "+");
//    m.add(fun<VecST, VecST, T>(maths::operator-), "-");
//    m.add(fun<VecST, VecST, T>(maths::operator*), "*");
//    m.add(fun<VecST, VecST, T>(maths::operator/), "/");
}

//----------------------------------------------------------------------------//

template <class T> inline
void impl_setup_Vector2(sol::state& /*lua*/, const String& /*name*/)
{
//    impl_setup_VectorST<2, T>(m, name);

//    m.add(constructor<maths::Vector2<T>(T, T)>(), name);

//    m.add(fun(&maths::Vector2<T>::x), "x");
//    m.add(fun(&maths::Vector2<T>::y), "y");
}

//----------------------------------------------------------------------------//

template <class T> inline
void impl_setup_Vector3(sol::state& /*lua*/, const String& /*name*/)
{
//    impl_setup_VectorST<3, T>(m, name);

//    m.add(constructor<maths::Vector3<T>(T, T, T)>(), name);

//    m.add(fun(&maths::Vector3<T>::x), "x");
//    m.add(fun(&maths::Vector3<T>::y), "y");
//    m.add(fun(&maths::Vector3<T>::z), "z");
}

//----------------------------------------------------------------------------//

template <class T> inline
void impl_setup_Vector4(sol::state& /*lua*/, const String& /*name*/)
{
//    impl_setup_VectorST<4, T>(m, name);

//    m.add(constructor<maths::Vector4<T>(T, T, T, T)>(), name);

//    m.add(fun(&maths::Vector4<T>::x), "x");
//    m.add(fun(&maths::Vector4<T>::y), "y");
//    m.add(fun(&maths::Vector4<T>::z), "z");
//    m.add(fun(&maths::Vector4<T>::w), "w");
}

//----------------------------------------------------------------------------//

} // anonymous namespace

//============================================================================//

void sq::lua_setup_maths(sol::state& /*lua*/)
{
//    auto m = std::make_shared<chai::Module>();

//    //--------------------------------------------------------//

//    impl_setup_Vector2<int>(*m, "Vec2I");
//    impl_setup_Vector3<int>(*m, "Vec3I");
//    impl_setup_Vector4<int>(*m, "Vec4I");

//    impl_setup_Vector2<uint>(*m, "Vec2U");
//    impl_setup_Vector3<uint>(*m, "Vec3U");
//    impl_setup_Vector4<uint>(*m, "Vec4U");

//    impl_setup_Vector2<float>(*m, "Vec2F");
//    impl_setup_Vector3<float>(*m, "Vec3F");
//    impl_setup_Vector4<float>(*m, "Vec4F");

//    //--------------------------------------------------------//

//    m->add(constructor<QuatF(const QuatF&)>(), "QuatF");
//    m->add(constructor<QuatF(float, float, float, float)>(), "QuatF");
//    m->add(constructor<QuatF(float, float, float)>(), "QuatF");
//    m->add(fun(&QuatF::operator=), "=");

//    m->add(vector_conversion<Vector<QuatF>>());
//    m->add(type_conversion<QuatF, String, String(*)(const QuatF&)>(&chai_string));;

//    m->add(fun(&QuatF::x), "x");
//    m->add(fun(&QuatF::y), "y");
//    m->add(fun(&QuatF::z), "z");
//    m->add(fun(&QuatF::w), "w");

//    //--------------------------------------------------------//

//    m->add(vector_conversion<Vector<int>>());
//    m->add(vector_conversion<Vector<uint>>());
//    m->add(vector_conversion<Vector<float>>());

//    //--------------------------------------------------------//

//    m->add(fun<Vec2F, Vec2F>(&maths::normalize), "normalize");
//    m->add(fun<Vec3F, Vec3F>(&maths::normalize), "normalize");
//    m->add(fun<Vec4F, Vec4F>(&maths::normalize), "normalize");
//    m->add(fun<QuatF, QuatF>(&maths::normalize), "normalize");

//    //--------------------------------------------------------//

//    engine.add(m);
}
