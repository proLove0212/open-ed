#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#ifndef M_PI
#define M_PI 3.1415926535
#endif

// 標準的な WAVE ヘッダ ( 44 byte )
typedef struct
{
    char riff[ 4 ];             // = "RIFF"
    unsigned int total_size;    // 全体サイズ
    char fmt[ 8 ];              // "WAVEfmt "
    unsigned int fmt_size;      // fmt チャンクサイズ
    unsigned short  format;     // フォーマットの種類
    unsigned short  channel;    // チャンネル
    unsigned int   rate;        // サンプリングレート
    unsigned int   avgbyte;     // rate * block
    unsigned short  block;      // channels * bit / 8
    unsigned short  bit;        // ビット数
    char data[ 4 ];             // = "data"
    unsigned int data_size;     // data チャンクサイズ
} WAVEFORMAT;

int main()
{
    FILE * fin = fopen( "D-FILTER-1-1.wav", "rb" );
    if( !fin ){
        fprintf( stderr, "ファイルオープンに失敗しました\n" );
        exit(EXIT_FAILURE);
    }

    WAVEFORMAT wavefmt;
    size_t ret = fread( &wavefmt, 1, sizeof( WAVEFORMAT ) - 8, fin );
    if( ret != sizeof( WAVEFORMAT ) -8 ){
       fprintf( stderr, "ヘッダが壊れています\n" );
       exit(EXIT_FAILURE);
    }

    // data までスキップ
    size_t pos = sizeof( WAVEFORMAT ) - 8;
    while( pos < 200 && ( wavefmt.data[0] != 'd' || wavefmt.data[1] != 'a' ) ){
        fseek( fin, pos++, SEEK_SET );
        ret = fread( wavefmt.data, 1, 4, fin );
    }
    ret = fread( &wavefmt.data_size, 1, sizeof( unsigned int ), fin );
    wavefmt.fmt_size = 16;

    if( wavefmt.data[ 0 ] != 'd' || wavefmt.data[ 1 ] != 'a' ){
        fprintf( stderr, "ヘッダが壊れています\n" );
        exit(EXIT_FAILURE);
    }
    if( wavefmt.channel != 1 ){
        fprintf( stderr, "ステレオ音声は使用できません\n" );
        exit(EXIT_FAILURE);
    }
    if( wavefmt.bit != 16 ){
        fprintf( stderr, "8bit音声は使用できません\n" );
        exit(EXIT_FAILURE);
    }

    fprintf( stderr,"fmt_size = %d\n", wavefmt.fmt_size );
    fprintf( stderr,"channel = %d\n", wavefmt.channel );
    fprintf( stderr,"rate = %d\n", wavefmt.rate );
    fprintf( stderr,"avgbyte = %d\n", wavefmt.avgbyte );
    fprintf( stderr,"block = %d\n", wavefmt.block );
    fprintf( stderr,"bit = %d\n", wavefmt.bit );   
    fprintf( stderr,"data_size = %d\n", wavefmt.data_size );

    short* buf = ( short* )malloc( 2 * 1024 * 1024 );
    ret = fread( buf , 1, wavefmt.data_size, fin );
    fclose( fin );

    size_t lng = wavefmt.data_size / wavefmt.block;
    fprintf( stderr,"length = %d\n", (int)lng);

    double* x_tmp = ( double* )malloc( lng * 2 * sizeof(double) );
    memset( x_tmp, 0, lng * 2 * sizeof(double) );
    double *x = x_tmp + lng;
    for( int i = 0; i < lng; ++i ) x[i] = (double)buf[i];

    double* y = ( double* )malloc( lng * sizeof(double) );
    memset( y, 0, lng * sizeof(double) );

    const int fs = wavefmt.rate;  // サンプリング周波数 [Hz]

    //----------------------------------------------
    // ここから穴埋め開始

    // x[i] を fs [Hz]で 5 秒間サンプリングした音声のディジタル信号とする
    // i の範囲は 0 <= i < N となる
    const int N = 5 * fs; // 5 [秒] * fs [Hz]

    // 乗算器の乗数 a と b をセット
    // a と b は負数でも可だが、|a| + |b| が 1.0 を超えないようにする
    const double a = ?;
    const double b = ?;

    // 遅延素子の遅延時刻 n の計算
    const double delay = ?; // 遅れ秒数
    const int n = (int)( delay * fs ); // n = 遅れ秒数 * fs

    // ディジタル線形フィルタ H(z) = a + b・z^{-n}
    // 出力信号は y[i]
    // i-n が負の場合は x[i-n] = 0 と既になっているので、場合分けせずにそのまま使って OK
    for( int i = 0; i < N; ++i ){
        y[i] = ?
    }

    // ここまで
    //----------------------------------------------

    FILE* fout = fopen( "D-FILTER-1-1-out.wav", "wb" );
    if( !fout ){
        fprintf( stderr, "ファイルオープンに失敗しました\n" );
        exit(EXIT_FAILURE);
    }
    for( int i = 0; i < lng; ++i ) buf[i] = (short)y[i];

    fwrite( &wavefmt, 1, sizeof( WAVEFORMAT ), fout );
    fwrite( buf, 1, wavefmt.data_size , fout );
    fclose( fout );
    
    return 0;
}
