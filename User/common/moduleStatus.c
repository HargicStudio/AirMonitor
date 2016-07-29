#include "moduleStatus.h"


u32 g_moduleStatus;

STATUS_e GetModuleStu(MODULE_e type)
{
    return ((0x3 << type) & g_moduleStatus) >> type;
}

void SetModuleStu(MODULE_e type, STATUS_e stu)
{
    (g_moduleStatus & (~(0x3 << type))) | ((stu & 0x3) << type);
}

u32 GetAllModuleStu(void)
{
    return g_moduleStatus;
}


