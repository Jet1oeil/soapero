/*
 * QWSDLParserHandler.cpp
 *
 *  Created on: 12 juil. 2017
 *      Author: lgruber
 */

#include <QBuffer>

#include "Model/ComplexType.h"
#include "Model/SimpleType.h"
#include "Model/Type.h"

#include "QWSDLParserHandler.h"

QWSDLParserHandler::QWSDLParserHandler()
{
	m_pListTypes = TypeList::create();
}

QWSDLParserHandler::~QWSDLParserHandler()
{

}

bool QWSDLParserHandler::startElement(const QString &namespaceURI,
		const QString &localName,
		const QString &qName,
		const QXmlAttributes &attributes)
{
	int iRes;

	if(qName == "xs:schema") {
		QString szTargetNamespace;

		if(attributes.index("targetNamespace") != -1) {
			szTargetNamespace = attributes.value("targetNamespace");
		}

		for(int i=0; i < attributes.length(); ++i) {
			qDebug("%s %s", qPrintable(attributes.qName(i)), qPrintable(attributes.value(i)));
			if(attributes.value(i) == szTargetNamespace && attributes.qName(i) != "targetNamespace") {
				m_szTargetNamespacePrefix = attributes.localName(i);
				qDebug("[QWSDLParserHandler::startElement] Target namespace prefix found: %s", qPrintable(m_szTargetNamespacePrefix));
			}
		}
	}

	if(qName == "xs:import") {
		iRes = attributes.index("schemaLocation");
		if(iRes != -1) {

			QString szLocation(attributes.value("schemaLocation"));
			QFile file(szLocation);

			if(file.open(QFile::ReadOnly)) {

				QByteArray bytes = file.readAll();
				QBuffer buffer;
				buffer.setData(bytes);

				QXmlInputSource source(&buffer);
				QXmlSimpleReader reader;
				QWSDLParserHandler handler;
				reader.setContentHandler(&handler);
				reader.setErrorHandler(&handler);
				reader.setFeature("http://xml.org/sax/features/namespace-prefixes", true);
				reader.setFeature("http://xml.org/sax/features/namespaces", true);
				if(reader.parse(source)){
					TypeListSharedPtr pList = handler.getTypeList();
					TypeList::const_iterator type;
					for(type = pList->constBegin(); type != pList->constEnd(); ++type) {
						m_pListTypes->add(*type);
					}
				}else{
					qWarning("[QWSDLParserHandler::startElement] Error to parse file %s (error: %s)",
							qPrintable(szLocation),
							qPrintable(reader.errorHandler()->errorString()));
				}
			}else{
				qWarning("[QWSDLParserHandler::startElement] Error for opening file %s (error: %s)",
						qPrintable(szLocation),
						qPrintable(file.errorString()));
			}
		}
	}



	if(qName == "xs:simpleType") {
		m_szCurrentSection = qName;

		iRes = attributes.index("name");
		if(iRes != -1) {
			QString szName = attributes.value("name");

			if(!m_pListTypes->getByName(szName, m_szTargetNamespacePrefix).isNull()) {
				m_pCurrentType = m_pListTypes->getByName(szName, m_szTargetNamespacePrefix);
			}else{
				m_pCurrentType = SimpleType::create();
				m_pCurrentType->setLocalName(szName);
				m_pCurrentType->setNamespace(m_szTargetNamespacePrefix);
			}


		}
	}

	if(qName == "xs:complexType") {
		m_szCurrentSection = qName;

		iRes = attributes.index("name");
		if(iRes != -1) {
			QString szName = attributes.value("name");

			if(!m_pListTypes->getByName(szName, m_szTargetNamespacePrefix).isNull()) {
				m_pCurrentType = m_pListTypes->getByName(szName, m_szTargetNamespacePrefix);
			}else{
				m_pCurrentType = ComplexType::create();
				m_pCurrentType->setLocalName(szName);
				m_pCurrentType->setNamespace(m_szTargetNamespacePrefix);
			}
		}
	}



	if(m_szCurrentSection == "xs:simpleType" && !m_pCurrentType.isNull()) {
		if(qName == "xs:restriction") {
			iRes = attributes.index("base");
			if(iRes != -1) {
				SimpleTypeSharedPtr pSimpleType = qSharedPointerCast<SimpleType>(m_pCurrentType);
				pSimpleType->setVariableTypeFromString(attributes.value("base"));
			}

		}
		if(qName == "xs:maxLength") {
			iRes = attributes.index("value");
			if(iRes != -1) {
				SimpleTypeSharedPtr pSimpleType = qSharedPointerCast<SimpleType>(m_pCurrentType);
				pSimpleType->setMaxLength(attributes.value("value").toUInt());
			}

		}
		if(qName == "xs:minLength") {
			iRes = attributes.index("value");
			if(iRes != -1) {
				SimpleTypeSharedPtr pSimpleType = qSharedPointerCast<SimpleType>(m_pCurrentType);
				pSimpleType->setMinLength(attributes.value("value").toUInt());
			}

		}
	}


	if(m_szCurrentSection == "xs:complexType" && !m_pCurrentType.isNull()) {
		if(qName == "xs:extension") {

			iRes = attributes.index("base");
			if(iRes != -1) {
				QString szName = attributes.value("base");
				if(!szName.startsWith("xs:")) {
					TypeSharedPtr pType = m_pListTypes->getByName(szName.split(":")[1], szName.split(":")[0]);
					qSharedPointerCast<ComplexType>(m_pCurrentType)->setExtensionType(pType);
				}
			}

		}

		if(qName == "xs:attribute") {
			AttributeSharedPtr attr = Attribute::create();

			iRes = attributes.index("name");
			if(iRes != -1) {
				attr->setName(attributes.value("name"));
			}

			iRes = attributes.index("type");
			if(iRes != -1) {
				QString szValue = attributes.value("type");
				QString szNamespace = szValue.split(":")[0];
				QString szLocalName = szValue.split(":")[1];
				TypeSharedPtr pType = m_pListTypes->getByName(szNamespace, szLocalName);
				if(!pType.isNull()){
					attr->setType(pType);
				}else{
					qWarning("[QWSDLParserHandler::endElement] Type %s is not found at this moment, we create it", qPrintable(szValue));

					if(szValue.startsWith("xs:")) {
						SimpleTypeSharedPtr pType = SimpleType::create();
						pType->setVariableTypeFromString(szValue);
						pType->setName(attr->getName());
						attr->setType(pType);
					}else{
						ComplexTypeSharedPtr pComplexType = ComplexType::create();
						pComplexType->setLocalName(szLocalName);
						pComplexType->setNamespace(szNamespace);
						m_pListTypes->append(pComplexType);
						attr->setType(pComplexType);
					}
				}
			}

			iRes = attributes.index("use");
			if(iRes != -1) {
				QString szValue = attributes.value("use");
				if(szValue == "required") {
					attr->setRequired(true);
				}
			}

			ComplexTypeSharedPtr pComplexType = qSharedPointerCast<ComplexType>(m_pCurrentType);
			pComplexType->addAttribute(attr);
		}
		if(qName == "xs:element") {
			ElementSharedPtr element = Element::create();

			iRes = attributes.index("name");
			if(iRes != -1) {
				element->setName(attributes.value("name"));
			}

			iRes = attributes.index("type");
			if(iRes != -1) {
				QString szValue = attributes.value("type");
				QString szNamespace = szValue.split(":")[0];
				QString szLocalName = szValue.split(":")[1];
				TypeSharedPtr pType = m_pListTypes->getByName(szNamespace, szLocalName);
				if(!pType.isNull()){
					element->setType(pType);
				}else{
					qWarning("[QWSDLParserHandler::endElement] Type %s is not found at this moment, we create it", qPrintable(szValue));

					if(szValue.startsWith("xs:")) {
						SimpleTypeSharedPtr pType = SimpleType::create();
						pType->setVariableTypeFromString(szValue);
						pType->setName(element->getName());
						element->setType(pType);
					}else{
						ComplexTypeSharedPtr pComplexType = ComplexType::create();
						pComplexType->setLocalName(szLocalName);
						pComplexType->setNamespace(szNamespace);
						m_pListTypes->append(pComplexType);
						element->setType(pComplexType);
					}
				}
			}


			ComplexTypeSharedPtr pComplexType = qSharedPointerCast<ComplexType>(m_pCurrentType);
			pComplexType->addElement(element);
		}
	}

	return true;
}

bool
QWSDLParserHandler::endElement(const QString &namespaceURI,
		const QString &localName,
		const QString &qName)
{

	if(qName == "xs:simpleType" && !m_pCurrentType.isNull()) {
		m_szCurrentSection = "";
		m_pListTypes->add(m_pCurrentType);
		m_pCurrentType.clear();

	}

	if(qName == "xs:complexType" && !m_pCurrentType.isNull()) {
		m_szCurrentSection = "";
		m_pListTypes->add(m_pCurrentType);
		m_pCurrentType.clear();

	}

	return true;
}

bool
QWSDLParserHandler::characters(const QString &str)
{
	m_szCurrentText = str.trimmed();
	return true;
}

bool
QWSDLParserHandler::fatalError(const QXmlParseException &exception)
{
	qWarning("[QWSDLParserHandler] Error while parsing XML content : (line %d) %s", exception.lineNumber(), qPrintable(exception.message()));
	return false;
}

TypeListSharedPtr QWSDLParserHandler::getTypeList() const
{
	return m_pListTypes;
}