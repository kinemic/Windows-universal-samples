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
#include "pch.h"
#include "Scenario1_Render.xaml.h"
#include <string>

using namespace SDKTemplate;

using namespace Concurrency;
using namespace Platform;
using namespace Windows::Data::Pdf;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Storage;
using namespace Windows::Storage::Pickers;
using namespace Windows::Storage::Streams;
using namespace Windows::UI::Core;
using namespace Windows::UI::ViewManagement;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media::Imaging;

Scenario1_Render::Scenario1_Render()
{
    InitializeComponent();

	engine = Kinemic::Gesture::Engine::Default;
	engineViewModel = Kinemic::Gesture::Common::EngineViewModel::Create(engine);
	bandViewModel = Kinemic::Gesture::Common::BandViewModel::Create(engine, "");

	engine->ConnectionStateChanged += ref new TypedEventHandler<Kinemic::Gesture::Engine^, Kinemic::Gesture::ConnectionStateChangedEventArgs^>(this, &Scenario1_Render::Engine_ConnectionStateChanged);
	engine->AirmouseMoved += ref new TypedEventHandler<Kinemic::Gesture::Engine^, Kinemic::Gesture::AirmouseMovedEventArgs^>(this, &Scenario1_Render::Engine_AirmouseMoved);
	engine->GestureDetected += ref new TypedEventHandler<Kinemic::Gesture::Engine^, Kinemic::Gesture::GestureDetectedEventArgs^>(this, &Scenario1_Render::Engine_GestureDetected);
}

void Scenario1_Render::Engine_ConnectionStateChanged(Kinemic::Gesture::Engine^ sender, Kinemic::Gesture::ConnectionStateChangedEventArgs^ e) {
	bandViewModel->Band = e->Band;
}

void Scenario1_Render::Engine_AirmouseMoved(Kinemic::Gesture::Engine^ sender, Kinemic::Gesture::AirmouseMovedEventArgs^ e) {
	if (airmouseState != e->PalmFacing) {
		lastX = e->X;
		lastY = e->Y;
		airmouseState = e->PalmFacing;

		bandViewModel->Vibrate(100);

		switch (airmouseState)
		{
		case Kinemic::Gesture::AirMousePalmFacing::PALM_FACING_DOWNWARDS:
			rootPage->NotifyUser("Panning", NotifyType::StatusMessage);
			break;
		case Kinemic::Gesture::AirMousePalmFacing::PALM_FACING_UPWARDS:
			rootPage->NotifyUser("Zooming", NotifyType::StatusMessage);
			break;
		case Kinemic::Gesture::AirMousePalmFacing::PALM_FACING_SIDEWAYS:
		case Kinemic::Gesture::AirMousePalmFacing::PALM_FACING_INCONCLUSIVE:
			rootPage->NotifyUser("", NotifyType::StatusMessage);
			break;
		}
	}
	else {
		float dx = e->X - lastX;
		float dy = e->Y - lastY;

		lastX = e->X;
		lastY = e->Y;

		switch (airmouseState) {
		case Kinemic::Gesture::AirMousePalmFacing::PALM_FACING_DOWNWARDS:
			/* Use dx and dy to pan the pdf (by panning the scroll view) */
			ScrollViewer->ChangeView(ScrollViewer->HorizontalOffset + dx * -1 * PAN_FACTOR, ScrollViewer->VerticalOffset + dy * PAN_FACTOR, nullptr );
			break;
		case Kinemic::Gesture::AirMousePalmFacing::PALM_FACING_UPWARDS:
			/* Use dy to zoom the pdf (by zooming the scroll view) */
			// TODO: zoom on center by adjusting offsets
			ScrollViewer->ChangeView(nullptr, nullptr, ScrollViewer->ZoomFactor + (dy * ZOOM_FACTOR));
			break;
		case Kinemic::Gesture::AirMousePalmFacing::PALM_FACING_SIDEWAYS:
			break;
		case Kinemic::Gesture::AirMousePalmFacing::PALM_FACING_INCONCLUSIVE:
			break;
		}
	}
}

void Scenario1_Render::Engine_GestureDetected(Kinemic::Gesture::Engine^ sender, Kinemic::Gesture::GestureDetectedEventArgs^ e) {
	unsigned long pageNumber;
	switch (e->Gesture) {
	case Kinemic::Gesture::Gesture::ROTATE_RL:
		/* KINEMIC: Activate airmouse on Rotate RL gesture */
		bandViewModel->IsAirmouseActive = true;
		bandViewModel->Vibrate(300);
		rootPage->NotifyUser("Airmouse Active", NotifyType::StatusMessage);
		break;
	case Kinemic::Gesture::Gesture::ROTATE_LR:
		/* KINEMIC: Deactivate airmouse on Rotate LR gesture */
		bandViewModel->IsAirmouseActive = false;
		bandViewModel->Vibrate(300);
		rootPage->NotifyUser("", NotifyType::StatusMessage);
		break;
	case Kinemic::Gesture::Gesture::CIRCLE_R:
		break;
	case Kinemic::Gesture::Gesture::CIRCLE_L:
		break;
	case Kinemic::Gesture::Gesture::SWIPE_R:
		/* KINEMIC: Next page on Swipe R */
		pageNumber = wcstoul(PageNumberBox->Text->Data(), nullptr, 10);
		if (pdfDocument != nullptr &&(pageNumber + 1 >= 1) && (pageNumber + 1 <= pdfDocument->PageCount))
		{
			PageNumberBox->Text = "" + (pageNumber + 1).ToString();
			ViewPage();
			bandViewModel->Vibrate(300);
		}
		else
		{
			bandViewModel->Vibrate(150);
		}
		break;
	case Kinemic::Gesture::Gesture::SWIPE_L:
		/* KINEMIC: Previous page on Swipe L */

		pageNumber = wcstoul(PageNumberBox->Text->Data(), nullptr, 10);
		if (pdfDocument != nullptr && (pageNumber - 1 >= 1) && (pageNumber - 1 <= pdfDocument->PageCount))
		{
			PageNumberBox->Text = "" + (pageNumber - 1).ToString();
			ViewPage();
			bandViewModel->Vibrate(300);
		}
		else
		{
			bandViewModel->Vibrate(150);
		}
		break;
	case Kinemic::Gesture::Gesture::SWIPE_UP:
		break;
	case Kinemic::Gesture::Gesture::SWIPE_DOWN:
		break;
	case Kinemic::Gesture::Gesture::CHECK_MARK:
		break;
	case Kinemic::Gesture::Gesture::CROSS_MARK:
		break;
	case Kinemic::Gesture::Gesture::EARTOUCH_R:
		break;
	case Kinemic::Gesture::Gesture::EARTOUCH_L:
		break;
	}
}

void Scenario1_Render::LoadDocument()
{
    LoadButton->IsEnabled = false;

    pdfDocument = nullptr;
    Output->Source = nullptr;
    PageNumberBox->Text = "1";
    RenderingPanel->Visibility = Windows::UI::Xaml::Visibility::Collapsed;

    auto picker = ref new FileOpenPicker();
    picker->FileTypeFilter->Append(".pdf");
    create_task(picker->PickSingleFileAsync()).then([this](StorageFile^ file)
    {
        if (file != nullptr)
        {
            ProgressControl->Visibility = Windows::UI::Xaml::Visibility::Visible;
            return create_task(PdfDocument::LoadFromFileAsync(file, PasswordBox->Password));
        }
        else
        {
            return task_from_result(static_cast<PdfDocument^>(nullptr));
        }
    }).then([this](task<PdfDocument^> task)
    {
        try
        {
            pdfDocument = task.get();
        }
        catch (Exception^ ex)
        {
            switch (ex->HResult)
            {
            case __HRESULT_FROM_WIN32(ERROR_WRONG_PASSWORD):
                rootPage->NotifyUser("Document is password-protected and password is incorrect.", NotifyType::ErrorMessage);
                break;

            case E_FAIL:
                rootPage->NotifyUser("Document is not a valid PDF.", NotifyType::ErrorMessage);
                break;

            default:
                // File I/O errors are reported as exceptions.
                rootPage->NotifyUser(ex->Message, NotifyType::ErrorMessage);
                break;
            }
        }

        if (pdfDocument != nullptr)
        {
            RenderingPanel->Visibility = Windows::UI::Xaml::Visibility::Visible;
            if (pdfDocument->IsPasswordProtected)
            {
                rootPage->NotifyUser("Document is password protected.", NotifyType::StatusMessage);
            }
            else
            {
                rootPage->NotifyUser("Document is not password protected.", NotifyType::StatusMessage);
            }
            PageCountText->Text = pdfDocument->PageCount.ToString();
        }
        ProgressControl->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
        LoadButton->IsEnabled = true;
    });
}

void Scenario1_Render::ViewPage()
{
    rootPage->NotifyUser("", NotifyType::StatusMessage);

    // If the text is not a valid number, then wcstoul returns 0, which is an invalid
    // page number, so we don't need to special-case that possibility.
    unsigned long pageNumber = wcstoul(PageNumberBox->Text->Data(), nullptr, 10);

    if ((pageNumber < 1) || (pageNumber > pdfDocument->PageCount))
    {
        rootPage->NotifyUser("Invalid page number.", NotifyType::ErrorMessage);
        return;
    }

    Output->Source = nullptr;
    ProgressControl->Visibility = Windows::UI::Xaml::Visibility::Visible;

    // Convert from 1-based page number to 0-based page index.
    unsigned long pageIndex = pageNumber - 1;

    PdfPage^ page = pdfDocument->GetPage(pageIndex);

    auto stream = ref new InMemoryRandomAccessStream();

    IAsyncAction^ renderAction;

    switch (Options->SelectedIndex)
    {
    // View actual size.
    case 0:
        renderAction = page->RenderToStreamAsync(stream);
        break;

    // View half size on beige background.
    case 1:
    {
        auto options1 = ref new PdfPageRenderOptions();
        options1->BackgroundColor = Windows::UI::Colors::Beige;
        options1->DestinationHeight = static_cast<unsigned int>(page->Size.Height / 2);
        options1->DestinationWidth = static_cast<unsigned int>(page->Size.Width / 2);
        renderAction = page->RenderToStreamAsync(stream, options1);
        break;
    }

    // Crop to center.
    case 2:
    {
        auto options2 = ref new PdfPageRenderOptions();
        auto rect = page->Dimensions->TrimBox;
        options2->SourceRect = Rect(rect.X + rect.Width / 4, rect.Y + rect.Height / 4, rect.Width / 2, rect.Height / 2);
        renderAction = page->RenderToStreamAsync(stream, options2);
        break;
    }
    }

    create_task(renderAction).then([this, stream]()
    {
        // Put the stream in the image.
        auto src = ref new BitmapImage();
        Output->Source = src;
        return create_task(src->SetSourceAsync(stream));
    }).then([this]()
    {
        ProgressControl->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
    });
}
