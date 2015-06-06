/*
 * =====================================================================================
 *
 *       Filename:  types.h
 *
 *    Description:  types 
 *
 *        Version:  1.0
 *        Created:  2014年09月19日 15时10分48秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  lizhixian (R&D), lzx1442@163.com
 *        Company:  wanwei-tech
 *
 * =====================================================================================
 */
#ifndef __TYPES_H__
#define __TYPES_H__

/**
 * \var typedef unsigned char BYTE
 * \brief A type definition for unsigned character (byte).
 */
typedef unsigned char   BYTE;
/**
 * \var typedef signed char BYTE
 * \brief A type definition for signed character.
 */
typedef signed char     SBYTE;
/**
 * \var typedef unsigned short WORD 
 * \brief A type definition for unsigned short integer (word).
 */
typedef unsigned short  WORD;
/**
 * \var typedef signed short SWORD 
 * \brief A type definition for signed short integer.
 */
typedef signed short    SWORD;
/**
 * \var typedef unsigned long DWORD
 * \brief A type definition for unsigned long integer (double word).
 */
typedef unsigned long   DWORD;
/**
 * \var typedef signed long SDWORD
 * \brief A type definition for signed long integer.
 */
typedef signed long     SDWORD;

/**
 * \var typedef unsigned int UINT
 * \brief A type definition for unsigned integer.
 */
typedef unsigned int    UINT;
/**
 * \var typedef long LONG
 * \brief A type definition for long integer.
 */
typedef long            LONG;

/**
 * \var typedef UINT WPARAM
 * \brief A type definition for the first message paramter.
 */
typedef UINT            WPARAM;
/**
 * \var typedef DWORD WPARAM
 * \brief A type definition for the second message paramter.
 */
typedef DWORD           LPARAM;

/**
* @brief 线程模块句柄
*/
typedef UINT HMOD;



/**
 * \def LOBYTE(w)
 * \brief Returns the low byte of the word \a w.
 *
 * \sa MAKEWORD
 */
#define LOBYTE(w)           ((BYTE)(w))
/**
 * \def HIBYTE(w)
 * \brief Returns the high byte of the word \a w.
 *
 * \sa MAKEWORD
 */
#define HIBYTE(w)           ((BYTE)(((WORD)(w) >> 8) & 0xFF))

/**
 * \def LOWORD(l)
 * \brief Returns the low word of the double word \a l
 *
 * \sa MAKELONG
 */
#define LOWORD(l)           ((WORD)(DWORD)(l))
/**
 * \def HIWORD(l)
 * \brief Returns the high word of the double word \a l
 *
 * \sa MAKELONG
 */
#define HIWORD(l)           ((WORD)((((DWORD)(l)) >> 16) & 0xFFFF))

#endif
