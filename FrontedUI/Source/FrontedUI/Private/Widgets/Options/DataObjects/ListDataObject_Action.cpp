// LeeHwaRang All Rights Reserved

#include "Widgets/Options/DataObjects/ListDataObject_Action.h"

void UListDataObject_Action::SetAction(TFunction<void()> InAction)
{
	BoundAction = MoveTemp(InAction);
}

void UListDataObject_Action::ExecuteAction() const
{
	if (BoundAction)
	{
		BoundAction();
	}
}
