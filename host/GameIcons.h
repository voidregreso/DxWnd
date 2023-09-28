

class CGameIcons {
public:
    HICON Extract(CString path);
    char *Serialize(HICON);
    HICON DeSerialize(char *);
    HICON GrayIcon(HICON);
private:
    //void char2hex(unsigned char *src, char *dst, int len);
    //int  hex2char(unsigned char *dst, char *src);
};
