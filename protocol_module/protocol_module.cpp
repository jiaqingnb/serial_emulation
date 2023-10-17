#include "protocol_module.h"
#include <QDebug>

static s_protocolmap protocolmap[15] = {
   {"0xa1",0xa1},{"0xa2",0xa2},{"0xa3",0xa3},{"0xa4",0xa4},{"0xa5",0xa5},{"0xa6",0xa6},{"0xa7",0xa7},{"0x18",0x18},{"0x19",0x19},
   {"SFP",APP_USE_SFP},{"RSSP-I",APP_USE_RSSP1},{"透传数据",APP_USE_NOT_PROTOCOL},
   {"50ms",0x32},{"100ms",0x64},{"200ms",0xc8}
};

protocol_module::protocol_module()
{

}

void protocol_module::protocol_init(s_protocol t_pro)
{
    qDebug()<<t_pro.device<<t_pro.protocol<<t_pro.cycle;
    uint32_t device;
    uint32_t protocol;
    uint32_t cycle;

    device = search_protocolmapping(t_pro.device);

    protocol = search_protocolmapping(t_pro.protocol);

    cycle = search_protocolmapping(t_pro.cycle);
    qDebug()<<device<<protocol<<cycle;
}

uint32_t protocol_module::search_protocolmapping(QString input)
{
    uint8_t i = 0;
    uint32_t output = 0xffffffff;
    for (i = 0;i<20;i++)
    {
        if(protocolmap[i].input == input)
        {
            output = protocolmap[i].output;
            break;
        }
    }
    return output;
}
