/*
 * Service.h
 *
 *  Created on: 25 juil. 2017
 *      Author: lgruber
 */

#ifndef SERVICE_H_
#define SERVICE_H_

#include "Operation.h"

class Service;
typedef QSharedPointer<Service> ServiceSharedPtr;

class Service
{
public:
	Service();
	virtual ~Service();

	static ServiceSharedPtr create();

	QString getName() const;
	void setName(const QString& szName);

	QString getTargetNamespace() const;
	void setTargetNamespace(const QString& szTargetNamespace);

	OperationListSharedPtr getOperationList() const;
	void addOperation(const OperationSharedPtr& pOperation);

private:
	OperationListSharedPtr m_pOperationList;

	QString m_szName;
	QString m_szTargetNamespace;

};




#endif /* SERVICE_H_ */
