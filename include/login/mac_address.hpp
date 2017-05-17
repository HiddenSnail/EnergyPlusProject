#ifndef MACADDRESSES_HPP
#define MACADDRESSES_HPP
#include "./include/global/stdafx.h"
#include <windows.h>
#include <wincon.h>
#include <stdio.h>
#include <nb30.h>
#define MACSESION 6
#pragma comment(lib,"Netapi32.lib")

typedef struct _ASTAT_
{
    ADAPTER_STATUS adapt;
    NAME_BUFFER NameBuff [30];
} ASTAT, *PASTAT;


std::string getmac_one( int lana_num )
{
    ASTAT Adapter;
    NCB ncb;
    UCHAR uRetCode;

    memset( &ncb, 0, sizeof( ncb ) );
    ncb.ncb_command = NCBRESET;
    ncb.ncb_lana_num = lana_num;

    uRetCode = Netbios( &ncb );
    //printf( "The NCBRESET return code is:0x%x \n", uRetCode );

    memset( &ncb, 0, sizeof( ncb ) );
    ncb.ncb_command = NCBASTAT;
    ncb.ncb_lana_num = lana_num;

    strcpy( (char *)ncb.ncb_callname, "* " );
    ncb.ncb_buffer = ( unsigned char * ) &Adapter;

    ncb.ncb_length = sizeof( Adapter );
    uRetCode = Netbios( &ncb );
    // printf( "The NCBASTAT return code is: 0x%x \n", uRetCode );
    std::string s;
    if ( uRetCode == 0 )
    {
        int bAddressInt [ MACSESION ];
        char CommarSeperatedAddress[ MACSESION * 3 ]={0};
        for( int i = 0; i < MACSESION; ++i )
        {
            bAddressInt[i] = Adapter.adapt.adapter_address[i];
            bAddressInt[i] &= 0x000000ff; // avoid "ff" leading bytes when "char" is lager then 0x7f
        }
        sprintf( CommarSeperatedAddress, "%02x:%02x:%02x:%02x:%02x:%02x",
                               bAddressInt[ 0 ],
                               bAddressInt[ 1 ],
                               bAddressInt[ 2 ],
                               bAddressInt[ 3 ],
                               bAddressInt[ 4 ],
                               bAddressInt[ 5 ]); // Should use scl::FormatString inside
        s = CommarSeperatedAddress;
    }
    return s;
}

MACAddresses GetNetBiosMacAddresses()
{
    NCB ncb;
    UCHAR uRetCode;
    LANA_ENUM lana_enum;
    memset( &ncb, 0, sizeof( ncb ) );
    ncb.ncb_command = NCBENUM;

    ncb.ncb_buffer = (unsigned char *) &lana_enum;
    ncb.ncb_length = sizeof( lana_enum );

    uRetCode = Netbios( &ncb );
    //printf( "The NCBENUM return code is:0x%x \n", uRetCode );
    MACAddresses vAdd;
    if ( uRetCode == 0 )
    {
        //printf( "Ethernet Count is : %d\n\n", lana_enum.length);
        for ( int i=0; i < lana_enum.length; ++i )
        {
            std::string s = getmac_one( lana_enum.lana[i] );
            if( ! s.empty() )
            {
                vAdd.push_back( s );
            }
        }
    }
    return vAdd;
}

#endif // MACADDRESSES_HPP
