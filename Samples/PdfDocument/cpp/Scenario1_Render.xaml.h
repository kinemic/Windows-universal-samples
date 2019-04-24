//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

#pragma once

#include "Scenario1_Render.g.h"
#include "MainPage.xaml.h"

namespace SDKTemplate
{
    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class Scenario1_Render sealed
    {
    public:
        Scenario1_Render();

        void LoadDocument();
        void ViewPage();

		property Kinemic::Gesture::Engine^ Engine
		{
			Kinemic::Gesture::Engine^ get() { return engine; }
		}

		property Kinemic::Gesture::Common::EngineViewModel^ EngineViewModel
		{
			Kinemic::Gesture::Common::EngineViewModel^ get() { return engineViewModel; }
		}

		property Kinemic::Gesture::Common::BandViewModel^ BandViewModel
		{
			Kinemic::Gesture::Common::BandViewModel^ get() { return bandViewModel; }
		}
    private:
		void Engine_ConnectionStateChanged(Kinemic::Gesture::Engine^ sender, Kinemic::Gesture::ConnectionStateChangedEventArgs^ e);
		void Engine_AirmouseMoved(Kinemic::Gesture::Engine^ sender, Kinemic::Gesture::AirmouseMovedEventArgs^ e);
		void Engine_GestureDetected(Kinemic::Gesture::Engine^ sender, Kinemic::Gesture::GestureDetectedEventArgs^ e);

        MainPage^ rootPage = MainPage::Current;
        Windows::Data::Pdf::PdfDocument^ pdfDocument;

		Kinemic::Gesture::Engine^ engine;
		Kinemic::Gesture::Common::BandViewModel^ bandViewModel;
		Kinemic::Gesture::Common::EngineViewModel^ engineViewModel;

		Kinemic::Gesture::AirMousePalmFacing airmouseState = Kinemic::Gesture::AirMousePalmFacing::PALM_FACING_INCONCLUSIVE;
		float lastX = 0.0f;
		float lastY = 0.0f;

		const float PAN_FACTOR = 14.0f;
		const float ZOOM_FACTOR = 0.03f;
    };
}
