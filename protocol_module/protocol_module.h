#ifndef PROTOCOL_MODULE_H
#define PROTOCOL_MODULE_H

#include <QString>

#define APP_USE_RSSP1 (1u)  /*使用RSSP-1协议 */
#define APP_USE_SFP (2u)    /*使用SFP协议 */
#define APP_USE_NOT_PROTOCOL (3u)  /*不使用任何协议 */

typedef struct
{
  QString input;
  uint32_t  output;
}s_protocolmap;

typedef struct
{
    QString protocol;
    QString device;
    QString cycle;
}s_protocol;

class protocol_module
{
public:

public:
    protocol_module();

    void protocol_init(s_protocol t_pro);//协议配置初始化
private:
    uint32_t search_protocolmapping(QString input);

};

#endif // PROTOCOL_MODULE_H
