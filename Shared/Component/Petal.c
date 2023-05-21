#include <Shared/Component/Petal.h>

#include <string.h>

#include <Shared/Encoder.h>
#include <Shared/Entity.h>
#include <Shared/SimulationCommon.h>

#ifdef RR_CLIENT
#include <Client/Renderer/Renderer.h>
#endif

enum
{
    state_flags_id =         0b000001,
    state_flags_rarity =     0b000010,
    state_flags_all =        0b000011
};

void rr_component_petal_init(struct rr_component_petal *this)
{
    memset(this, 0, sizeof *this);
}

void rr_component_petal_free(struct rr_component_petal *this)
{
}

#ifdef RR_SERVER
void rr_component_petal_write(struct rr_component_petal *this, struct rr_encoder *encoder, int is_creation)
{
    uint64_t state = this->protocol_state | (state_flags_all * is_creation);
    rr_encoder_write_varuint(encoder, state);
    RR_ENCODE_PUBLIC_FIELD(id, uint8);
    RR_ENCODE_PUBLIC_FIELD(rarity, uint8);
}

RR_DEFINE_PUBLIC_FIELD(petal, uint8_t, id)
RR_DEFINE_PUBLIC_FIELD(petal, uint8_t, rarity)
#endif

#ifdef RR_CLIENT
void rr_component_petal_read(struct rr_component_petal *this, struct rr_encoder *encoder)
{
    uint64_t state = rr_encoder_read_varuint(encoder);
    RR_DECODE_PUBLIC_FIELD(id, uint8);
    RR_DECODE_PUBLIC_FIELD(rarity, uint8);
}
#endif

#ifdef RR_CLIENT
void rr_component_petal_render(EntityIdx entity, struct rr_simulation *simulation, struct rr_renderer *renderer)
{
    struct rr_component_physical *physical = rr_simulation_get_physical(simulation, entity);
    rr_renderer_translate(renderer, physical->x, physical->y);
    rr_renderer_begin_path(renderer);
    rr_renderer_arc(renderer, 0, 0, physical->radius + 1.5);
    rr_renderer_set_fill(renderer, 0xffcfbb50);
    rr_renderer_fill(renderer);
}
#endif