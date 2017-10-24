#define _X86_ // こんなこと書いていいのか？

#include "Serial.h"
#include <algorithm>
#include <exception>
#include <wtypes.h>
#include <boost/format.hpp>

namespace
{
    /**
    * PortInfoを元にDCBを取得
    */
    DCB getDcd(PortInfo const & info)
    {
        DCB dcb = { 0 };
        dcb.DCBlength = sizeof(DCB);
        dcb.BaudRate = info.m_baudrate;
        dcb.ByteSize = info.m_bytesize;
        dcb.Parity = info.m_parity;
        dcb.StopBits = info.m_stopbits;
        return dcb;
    }
    /**
    * CreateFileが認識できるポート名を取得
    * @param[in] port ポート番号
    * @return CreateFileが認識できるポート名
    * @note 引数が不正の場合例外を投げる
    */
    std::wstring getPortName(int port)
    {
        if (port <= 0 || 99<port){
            throw std::runtime_error((boost::format("Invalid port [%d]") % port).str());
        }
        auto form = port < 10 ? L"COM%d" : L"//.////COM%d";
        return (boost::wformat(form) % port).str();
    }
}

Serial::Serial()
    : m_com(INVALID_HANDLE_VALUE)
{
}

Serial::~Serial()
{
    Close();
}

void Serial::Open(PortInfo const & info)
{
    Close();
    m_com = CreateFile(getPortName(info.m_port).c_str(),
        GENERIC_READ | GENERIC_WRITE, 0, NULL,
        OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (INVALID_HANDLE_VALUE == m_com){
        throw std::runtime_error((boost::format("Failed to open port %d") % info.m_port).str());
    }
    DCB dcb = getDcd(info);
    if (!SetCommState(m_com, &dcb)){
        throw std::runtime_error("Function failed. [SetCommState]");
    }
}

void Serial::Close()
{
    if (INVALID_HANDLE_VALUE != m_com){
        if (!CloseHandle(m_com)){
            throw std::runtime_error("Function failed CloseHandle");
        }
    }
}

bool Serial::Send(std::vector<unsigned char> const & data)const
{
    DWORD send_size = 0;
    return !!WriteFile(m_com, &data.front(), data.size(), &send_size, 0) && send_size == data.size();
}

bool Serial::Receive(std::vector<unsigned char> & buffer)const
{
    DWORD dwError;
    COMSTAT comStat;
    if (INVALID_HANDLE_VALUE == m_com ||
        !ClearCommError(m_com, &dwError, &comStat) ||
        comStat.cbInQue <= 0)
    {
        return false;
    }
    buffer.resize(comStat.cbInQue);
    DWORD read_size = 0;
    return ReadFile(m_com, &buffer.front(), buffer.size(), &read_size, 0) && read_size == buffer.size();
}

bool Serial::Connected()const
{
    return m_com != INVALID_HANDLE_VALUE;
}
