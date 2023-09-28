#include "stdafx.h"
#include "dxwndhost.h"

extern void HandleExpertFields(CDialog *tab, int *res) {
    CWnd *item;
    for( ; *res; res++ ) {
        item = tab->GetDlgItem(*res);
        //if(!item) continue;
        if(gbExpertMode) {
            item->EnableWindow(TRUE);
            item->ShowWindow(SW_SHOW);
        } else {
            item->EnableWindow(FALSE);
            item->ShowWindow(SW_HIDE);
        }
    }
}


