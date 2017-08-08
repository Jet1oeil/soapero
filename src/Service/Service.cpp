/*
 * Service.cpp
 *
 *  Created on: 3 août 2017
 *      Author: lgruber
 */

#include <QDateTime>
#include <QCryptographicHash>

#include "Service.h"

namespace Onvif {

Service::Service()
{
	m_iLastErrorCode = -1;
}

Service::~Service()
{

}

void Service::setUrl(const QUrl& url)
{
	m_url = url;
	m_url.setPath("/onvif/device_service");
}

int Service::lastErrorCode() const
{
	return m_iLastErrorCode;
}

QString Service::lastError() const
{
	return m_szLastError;
}

QNetworkRequest Service::buildNetworkRequest() const
{
	QNetworkRequest request(m_url);
	return request;
}

QByteArray Service::buildSoapMessage(const QString& szSerializedObject) const
{
	QString szDatetime = QDateTime::currentDateTime().toString(Qt::ISODate);
	QString szNonce = buildNonce();
	QByteArray szNonce64 = szNonce.toLatin1().toBase64();
	QByteArray digestbytes = szNonce.toLatin1();
	digestbytes.append(szDatetime);
	digestbytes.append(m_url.password());
	QString szDigestPassword = QString(QCryptographicHash::hash(digestbytes, QCryptographicHash::Sha1).toBase64());

	QByteArray bytes;
	bytes += "<s:Envelope xmlns:s=\"http://www.w3.org/2003/05/soap-envelope\">";
	bytes +=     "<s:Header>";
	bytes +=         "<Security s:mustUnderstand=\"1\" xmlns=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-secext-1.0.xsd\">";
	bytes +=             "<UsernameToken>";
	bytes +=                 "<Username>" + m_url.userName() + "</Username>";
	bytes +=                 "<Password Type=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-username-token-profile-1.0#PasswordDigest\">" + szDigestPassword +  "</Password>";
	bytes +=                 "<Nonce EncodingType=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-soap-message-security-1.0#Base64Binary\">" + szNonce64 +  "</Nonce>";
	bytes +=                 "<Created xmlns=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-utility-1.0.xsd\">" + szDatetime +  "</Created>";
	bytes +=             "</UsernameToken>";
	bytes +=         "</Security>";
	bytes +=     "</s:Header>";
	bytes +=     "<s:Body xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\">";
	bytes +=		szSerializedObject;
	bytes +=     "</s:Body>";
	bytes += "</s:Envelope>";
	return bytes;
}

QString Service::buildNonce() const
{
	const QString possibleCharacters("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789");
	const int randomStringLength = 20;

	QString randomString;
	for(int i=0; i<randomStringLength; ++i) {
		int index = qrand() % possibleCharacters.length();
		QChar nextChar = possibleCharacters.at(index);
		randomString.append(nextChar);
	}
	return randomString;
}

}
