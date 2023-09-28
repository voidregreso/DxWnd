typedef char *(*Geterrwarnmessage_Type)(unsigned long, unsigned long);
typedef int (*Preparedisasm_Type)(void);
typedef void (*Finishdisasm_Type)(void);
typedef unsigned long (*Disasm_Type)(const unsigned char *, unsigned long, unsigned long, t_disasm *, int, t_config *, int (*)(tchar *, unsigned long));

extern Geterrwarnmessage_Type pGeterrwarnmessage;
extern Preparedisasm_Type pPreparedisasm;
extern Finishdisasm_Type pFinishdisasm;
extern Disasm_Type pDisasm;

extern HMODULE LoadDisasm(void);
