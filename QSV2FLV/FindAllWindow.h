#pragma once

//查找所有子窗口,深度遍历
HWND FindAllWindow(LPCTSTR lpClassName, LPCTSTR lpWindowName);

//根据进程名，返回第一个进程实例的PID
INT GetProcessID(LPCTSTR pzProcessName);
