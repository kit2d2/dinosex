#include <Shared/Component/Arena.h>

#include <string.h>

#include <Shared/pb.h>
#include <Shared/Entity.h>
#include <Shared/SimulationCommon.h>

#define FOR_EACH_PUBLIC_FIELD \
    X(radius, float32) 

enum
{
    state_flags_radius = 0b000001,
    state_flags_all =    0b000001
};

void rr_component_arena_init(struct rr_component_arena *this)
{
    memset(this, 0, sizeof *this);
}

void rr_component_arena_free(struct rr_component_arena *this, struct rr_simulation *simulation)
{
}

#ifdef RR_SERVER
void rr_component_arena_write(struct rr_component_arena *this, struct proto_bug *encoder, int is_creation)
{
    uint64_t state = this->protocol_state | (state_flags_all * is_creation);
    proto_bug_write_varuint(encoder, state, "arena component state");
#define X(NAME, TYPE) RR_ENCODE_PUBLIC_FIELD(NAME, TYPE);
    FOR_EACH_PUBLIC_FIELD
#undef X
}

RR_DEFINE_PUBLIC_FIELD(arena, float, radius)
#endif

#ifdef RR_CLIENT
void rr_component_arena_read(struct rr_component_arena *this, struct proto_bug *encoder)
{
    uint64_t state = proto_bug_read_varuint(encoder, "arena component state");
#define X(NAME, TYPE) RR_DECODE_PUBLIC_FIELD(NAME, TYPE);
    FOR_EACH_PUBLIC_FIELD
#undef X
}
#endif