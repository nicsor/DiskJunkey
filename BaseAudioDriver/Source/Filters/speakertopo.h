
/*++

Copyright (c) Microsoft Corporation All Rights Reserved

Module Name:

    speakertopo.h

Abstract:

    Declaration of topology miniport for the speaker (internal).
--*/

#ifndef _BASEAUDIODRIVER_SPEAKERTOPO_H_
#define _BASEAUDIODRIVER_SPEAKERTOPO_H_

NTSTATUS PropertyHandler_SpeakerTopoFilter(_In_ PPCPROPERTY_REQUEST PropertyRequest);

NTSTATUS PropertyHandler_SpeakerTopology(_In_ PPCPROPERTY_REQUEST PropertyRequest);

#endif // _BASEAUDIODRIVER_SPEAKERTOPO_H_
