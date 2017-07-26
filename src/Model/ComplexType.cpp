/*
 * ComplexType.cpp
 *
 *  Created on: 19 juil. 2017
 *      Author: lgruber
 */

#include "ComplexType.h"

#ifndef CRLF
#define CRLF "\r\n"
#endif

Element::Element()
{

}

Element::~Element()
{

}

ElementSharedPtr Element::create()
{
	return ElementSharedPtr(new Element());
}

void Element::setName(const QString& szName)
{
	m_szName = szName;
}

QString Element::getName() const
{
	return m_szName;
}

void Element::setType(const TypeSharedPtr& pType)
{
	m_pType = pType;
}

TypeSharedPtr Element::getType() const
{
	return m_pType;
}

ElementList::ElementList()
{

}

ElementList::~ElementList()
{

}

ElementListSharedPtr ElementList::create()
{
	return ElementListSharedPtr(new ElementList());
}

Attribute::Attribute()
{
	m_bRequired = false;
}

Attribute::~Attribute()
{

}

AttributeSharedPtr Attribute::create()
{
	return AttributeSharedPtr(new Attribute());
}

void Attribute::setName(const QString& szName)
{
	m_szName = szName;
}

QString Attribute::getName() const
{
	return m_szName;
}

void Attribute::setType(const TypeSharedPtr& pType)
{
	m_pType = pType;
}

TypeSharedPtr Attribute::getType() const
{
	return m_pType;
}

void Attribute::setRequired(bool bRequired)
{
	m_bRequired = bRequired;
}

bool Attribute::isRequired() const
{
	return m_bRequired;
}


AttributeList::AttributeList()
	:QList<AttributeSharedPtr>()
{

}

AttributeList::~AttributeList()
{

}

AttributeListSharedPtr AttributeList::create()
{
	return AttributeListSharedPtr(new AttributeList());
}

/*********************************************************
 *  COMPLEX TYPE
 */

ComplexType::ComplexType()
	:Type(Type::ComplexType)
{
	m_pListAttribute = AttributeList::create();
	m_pListElement = ElementList::create();
}

ComplexType::~ComplexType()
{

}

ComplexTypeSharedPtr ComplexType::create()
{
	return ComplexTypeSharedPtr(new ComplexType());
}

void ComplexType::setExtensionType(TypeSharedPtr pType)
{
	m_pExtensionType = pType;
}

TypeSharedPtr ComplexType::getExtensionType() const
{
	return m_pExtensionType;
}

void ComplexType::addAttribute(const AttributeSharedPtr& pAttribute)
{
	m_pListAttribute->append(pAttribute);
}

AttributeListSharedPtr ComplexType::getAttributeList() const
{
	return m_pListAttribute;
}

void ComplexType::addElement(const ElementSharedPtr& pElement)
{
	m_pListElement->append(pElement);
}

ElementListSharedPtr ComplexType::getElementList() const
{
	return m_pListElement;
}

QString ComplexType::getSetterDeclaration() const
{
	QString szVarName = getLocalName().left(1).toLower() + getLocalName().mid(1);

	QString szDeclaration = "void set%0(const %1& %2);";
	return szDeclaration.arg(getLocalName()).arg(getQualifiedName()).arg(szVarName);
}

QString ComplexType::getGetterDeclaration() const
{
	QString szDeclaration = "%0 get%1() const;";
	return szDeclaration.arg(getQualifiedName()).arg(getLocalName());
}

QString ComplexType::getSetterDefinition(const QString& szClassname) const
{
	QString szVarName = getLocalName().left(1).toLower() + getLocalName().mid(1);

	QString szDeclaration = ""
	"void %0::set%1(const %2& %3)" CRLF
	"{" CRLF
	"\t%4 = %5;" CRLF
	"}" CRLF;

	return szDeclaration.arg(szClassname).arg(getLocalName()).arg(getQualifiedName())
			.arg(szVarName).arg(getVariableName()).arg(szVarName);
}

QString ComplexType::getGetterDefinition(const QString& szClassname) const
{
	QString szDefinition = ""
	"void %0::get%1() const" CRLF
	"{" CRLF
	"\treturn %2;" CRLF
	"}" CRLF;

	return szDefinition.arg(szClassname).arg(getLocalName()).arg(getVariableName());
}

QString ComplexType::getVariableDeclaration() const
{
	QString szDeclaration;
	szDeclaration += getQualifiedName();
	szDeclaration += " ";
	szDeclaration += getVariableName();

	return szDeclaration;
}

QString ComplexType::getVariableName() const
{
	return "_" + getLocalName().left(1).toLower() + getLocalName().mid(1);
}

ComplexTypeList::ComplexTypeList()
	:QList<ComplexTypeSharedPtr>()
{

}

ComplexTypeList::~ComplexTypeList()
{

}

ComplexTypeListSharedPtr ComplexTypeList::create()
{
	return ComplexTypeListSharedPtr(new ComplexTypeList());
}

