#include "alt0cmd.h"

void cmdMake(CmdTypeId type, CmdGroupId group, char param, char id, unsigned short data, char* ret)
{
    ret[0] = type;
    ret[1] = group;
    ret[2] = param;
    ret[3] = id;
    ret[4] = (data>>8) & 0xff;
    ret[5] = data & 0xff;
}

void cmdDecode(char* cmd, char* type, char* group, char* param, char* id, unsigned short* data)
{
    *type = cmd[0];
    *group = cmd[1];
    *param = cmd[2];
    *id = cmd[3];
    *data = cmd[4] << 8 | cmd[5];
}
