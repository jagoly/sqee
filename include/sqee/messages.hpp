#pragma once

namespace sq { class Entity; }

namespace msg {

struct Create_Entity { sq::Entity* const entity; };
struct Configure_Entity { sq::Entity* const entity; };
struct Destroy_Entity { sq::Entity* const entity; };

}
