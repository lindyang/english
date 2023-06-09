#ifndef REQUESTS_H
#define REQUESTS_H
#include <QSsl>
#include <QSslSocket>
#include <QSslConfiguration>

#if defined(Q_OS_LINUX)
    const auto sslProtocol = QSsl::TlsV1SslV3;
#else
    const auto sslProtocol = QSsl::AnyProtocol;
#endif



inline QSslConfiguration getSslConf() {
    QSslConfiguration sslConfig = QSslConfiguration::defaultConfiguration();
    sslConfig.setProtocol(sslProtocol);
    sslConfig.setPeerVerifyMode(QSslSocket::VerifyNone);
    return sslConfig;
}


#endif // REQUESTS_H
