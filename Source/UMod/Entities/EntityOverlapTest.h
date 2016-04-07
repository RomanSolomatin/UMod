#pragma once
#include "EntityBase.h"
#include "EntityOverlapTest.generated.h"

UCLASS()
class AEntityOverlapTest : public AEntityBase {
	GENERATED_BODY()
public:
	AEntityOverlapTest();
	virtual void OnInit();
	virtual void OnBeginOverlap(AEntityBase *other);
	virtual void OnEndOverlap(AEntityBase *other);
};