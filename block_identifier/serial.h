#pragma once

#include <vector>

/*!
ポート情報
*/
struct PortInfo
{
    int port; ///< ポート番号
    int baudrate; ///< ボーレート
    unsigned char bytesize; ///< バイト長
    unsigned char parity; ///< パリティ
    unsigned char stopbits; ///< ストップビット
};

/*!
シリアルポートクラス
*/
class Serial
{
    Serial & operator=(Serial const &) = delete;
    Serial(Serial const &) = delete;
    void * m_com;
public:
    Serial();
    ~Serial();

    /**
    * COMポートをオープンする
    * @param[in] info	COMポート情報
    */
    void open(PortInfo const & info);

    /**
    * COMポートを閉じる
    */
    void close();

    /**
    * データを送信する
    * @param[in] data 送信するデータ
    * @return 成功・失敗
    */
    bool send(std::vector<unsigned char> const & data)const;

    /**
    * データを受信する
    * @param[in] buffer 受信バッファ
    * @retval true 成功
    * @retval false 失敗
    */
    bool receive(std::vector<unsigned char> & buffer)const;

    /**
    * COMポートの状態を返す
    * @retval true COMポートがオープンしている
    * @retval false COMポートがオープンしていない
    * @note Open中にケーブルを抜いた、デバイスの電源を切ったなどの事象が発生したことは検知できない
    */
    bool connected()const;
};
