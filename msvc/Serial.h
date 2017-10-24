#pragma once

#include <vector>

/*!
�|�[�g���
*/
struct PortInfo
{
    int m_port; ///< �|�[�g�ԍ�
    int m_baudrate; ///< �{�[���[�g
    unsigned char m_bytesize; ///< �o�C�g��
    unsigned char m_parity; ///< �p���e�B
    unsigned char m_stopbits; ///< �X�g�b�v�r�b�g
};

/*!
�V���A���|�[�g�N���X
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
    * COM�|�[�g���I�[�v������
    * @param[in] info	COM�|�[�g���
    */
    void Open(PortInfo const & info);

    /**
    * COM�|�[�g�����
    */
    void Close();

    /**
    * �f�[�^�𑗐M����
    * @param[in] data ���M����f�[�^
    * @return �����E���s
    */
    bool Send(std::vector<unsigned char> const & data)const;

    /**
    * �f�[�^����M����
    * @param[in] buffer ��M�o�b�t�@
    * @retval true ����
    * @retval false ���s
    */
    bool Receive(std::vector<unsigned char> & buffer)const;

    /**
    * COM�|�[�g�̏�Ԃ�Ԃ�
    * @retval true COM�|�[�g���I�[�v�����Ă���
    * @retval false COM�|�[�g���I�[�v�����Ă��Ȃ�
    * @note Open���ɃP�[�u���𔲂����A�f�o�C�X�̓d����؂����Ȃǂ̎��ۂ������������Ƃ͌��m�ł��Ȃ�
    */
    bool Connected()const;
};
