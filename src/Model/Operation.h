/*
 * Operation.h
 *
 *  Created on: 25 juil. 2017
 *      Author: lgruber
 */

#ifndef OPERATION_H_
#define OPERATION_H_

#include <QSharedPointer>

#include "Message.h"

class Operation;
typedef QSharedPointer<Operation> OperationSharedPtr;
class OperationList;
typedef QSharedPointer<OperationList> OperationListSharedPtr;


class Operation
{
public:
	Operation();
	virtual ~Operation();

	static OperationSharedPtr create();

	void setName(const QString& szName);
	QString getName() const;

	void setInputMessage(const MessageSharedPtr& pMessage);
	MessageSharedPtr getInputMessage() const;

	void setOutputMessage(const MessageSharedPtr& pMessage);
	MessageSharedPtr getOutputMessage() const;

	void setSoapEnvelopeFaultType(const ComplexTypeSharedPtr& pSoapEnvFaultType);
	const ComplexTypeSharedPtr& getSoapEnvelopeFaultType() const;

	void setSoapAction(const QString& szSoapAction);
	QString getSoapAction() const;

	QString getOperationDeclaration() const;
	QString getOperationDefinition(const QString& szClassname, const QString& szNamespace) const;

private:
	QString m_szName;
	QString m_szSoapAction;

	MessageSharedPtr m_pInputMessage;
	MessageSharedPtr m_pOutputMessage;

	ComplexTypeSharedPtr m_pSoapEnvFaultType;
};

class OperationList : public QList<OperationSharedPtr>
{
public:
	OperationList();
	virtual ~OperationList();

	static OperationListSharedPtr create();

	OperationSharedPtr getByName(const QString& szName);
};

#endif /* OPERATION_H_ */
