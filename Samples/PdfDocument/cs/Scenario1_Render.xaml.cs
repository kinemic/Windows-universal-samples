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

using Kinemic.Gesture;
using Kinemic.Gesture.Common;
using System;
using System.Threading.Tasks;
using Windows.Data.Pdf;
using Windows.Foundation;
using Windows.Storage;
using Windows.Storage.Pickers;
using Windows.Storage.Streams;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Media.Imaging;

namespace SDKTemplate
{
    public sealed partial class Scenario1_Render : Page
    {
        private MainPage rootPage = MainPage.Current;
        private PdfDocument pdfDocument;
        private Engine engine;
        private BandViewModel bandViewModel;
        private EngineViewModel engineViewModel;
        private AirMousePalmFacing airmouseState = AirMousePalmFacing.PALM_FACING_INCONCLUSIVE;
        private float lastX, lastY;

        const int WrongPassword = unchecked((int)0x8007052b); // HRESULT_FROM_WIN32(ERROR_WRONG_PASSWORD)
        const int GenericFail = unchecked((int)0x80004005);   // E_FAIL

        const float PAN_FACTOR = 14.0f;
        const float ZOOM_FACTOR = 0.03f;

        public Scenario1_Render()
        {
            this.InitializeComponent();

            /* KINEMIC: setup engine and view models */
            this.engine = Engine.Default;

            this.bandViewModel = BandViewModel.Create(this.engine, "");
            this.engineViewModel = EngineViewModel.Create(this.engine);

            this.engine.ConnectionStateChanged += Engine_ConnectionStateChanged;
            this.engine.AirmouseMoved += Engine_AirmouseMoved;
            this.engine.GestureDetected += Engine_GestureDetected;
        }


        /* KINEMIC: Properties to give access in xaml bindings */
        private Engine Engine { get => engine; }
        private BandViewModel BandViewModel { get => bandViewModel; }
        private EngineViewModel EngineViewModel { get => engineViewModel; }

        private void Engine_ConnectionStateChanged(Engine sender, ConnectionStateChangedEventArgs e)
        {
            /* KINEMIC: Since we only allow a single band, be update the band view model with the current one. */
            bandViewModel.Band = e.Band;
        }

        private void Engine_AirmouseMoved(Engine sender, AirmouseMovedEventArgs e)
        {
            /* KINEMIC: We use the three different palm facings for panning, zooming and repositioning (doing nothing) */
            if (airmouseState != e.PalmFacing)
            {
                lastX = e.X;
                lastY = e.Y;
                airmouseState = e.PalmFacing;

                bandViewModel.Vibrate(100);

                switch (airmouseState)
                {
                    case AirMousePalmFacing.PALM_FACING_DOWNWARDS:
                        rootPage.NotifyUser("Panning", NotifyType.StatusMessage);
                        break;
                    case AirMousePalmFacing.PALM_FACING_UPWARDS:
                        rootPage.NotifyUser("Zooming", NotifyType.StatusMessage);
                        break;
                    case AirMousePalmFacing.PALM_FACING_SIDEWAYS:
                    case AirMousePalmFacing.PALM_FACING_INCONCLUSIVE:
                        rootPage.NotifyUser("", NotifyType.StatusMessage);
                        break;
                }
            }
            else
            {
                float dx = e.X - lastX;
                float dy = e.Y - lastY;

                lastX = e.X;
                lastY = e.Y;

                switch (airmouseState)
                {
                    case AirMousePalmFacing.PALM_FACING_DOWNWARDS:
                        /* Use dx and dy to pan the pdf (by panning the scroll view) */
                        ScrollViewer.ChangeView(ScrollViewer.HorizontalOffset + dx*-1*PAN_FACTOR, ScrollViewer.VerticalOffset + dy * PAN_FACTOR, null);
                        break;
                    case AirMousePalmFacing.PALM_FACING_UPWARDS:
                        /* Use dy to zoom the pdf (by zooming the scroll view) */
                        // TODO: zoom on center by adjusting offsets
                        ScrollViewer.ChangeView(null, null, ScrollViewer.ZoomFactor + (dy * ZOOM_FACTOR));
                        break;
                    case AirMousePalmFacing.PALM_FACING_SIDEWAYS:
                        break;
                    case AirMousePalmFacing.PALM_FACING_INCONCLUSIVE:
                        break;
                }
            }
        }

        private void Engine_GestureDetected(Engine sender, GestureDetectedEventArgs e)
        {
            uint pageNumber;
            switch (e.Gesture)
            {
                case Gesture.ROTATE_RL:
                    /* KINEMIC: Activate airmouse on Rotate RL gesture */
                    bandViewModel.IsAirmouseActive = true;
                    bandViewModel.Vibrate(300);
                    rootPage.NotifyUser("Airmouse Active", NotifyType.StatusMessage);
                    break;
                case Gesture.ROTATE_LR:
                    /* KINEMIC: Deactivate airmouse on Rotate LR gesture */
                    bandViewModel.IsAirmouseActive = false;
                    bandViewModel.Vibrate(300);
                    rootPage.NotifyUser("", NotifyType.StatusMessage);
                    break;
                case Gesture.CIRCLE_R:
                    break;
                case Gesture.CIRCLE_L:
                    break;
                case Gesture.SWIPE_R:
                    /* KINEMIC: Next page on Swipe R */
                    if (pdfDocument != null && uint.TryParse(PageNumberBox.Text, out pageNumber) && (pageNumber + 1 >= 1) && (pageNumber + 1 <= pdfDocument.PageCount))
                    {
                        PageNumberBox.Text = "" + (pageNumber + 1);
                        ViewPage();
                        bandViewModel.Vibrate(300);
                    }
                    else
                    {
                        bandViewModel.Vibrate(150);
                    }
                    break;
                case Gesture.SWIPE_L:
                    /* KINEMIC: Previous page on Swipe L */
                    if (pdfDocument != null && uint.TryParse(PageNumberBox.Text, out pageNumber) && (pageNumber - 1 >= 1) && (pageNumber - 1 <= pdfDocument.PageCount))
                    {
                        PageNumberBox.Text = "" + (pageNumber - 1);
                        ViewPage();
                        bandViewModel.Vibrate(300);
                    } else
                    {
                        bandViewModel.Vibrate(150);
                    }
                    break;
                case Gesture.SWIPE_UP:
                    break;
                case Gesture.SWIPE_DOWN:
                    break;
                case Gesture.CHECK_MARK:
                    break;
                case Gesture.CROSS_MARK:
                    break;
                case Gesture.EARTOUCH_R:
                    break;
                case Gesture.EARTOUCH_L:
                    break;
            }
        }
        
        private async void LoadDocument()
        {
            LoadButton.IsEnabled = false;

            pdfDocument = null;
            Output.Source = null;
            PageNumberBox.Text = "1";
            RenderingPanel.Visibility = Visibility.Collapsed;

            var picker = new FileOpenPicker();
            picker.FileTypeFilter.Add(".pdf");
            StorageFile file = await picker.PickSingleFileAsync();
            if (file != null)
            {
                ProgressControl.Visibility = Visibility.Visible;
                try
                {
                    pdfDocument = await PdfDocument.LoadFromFileAsync(file, PasswordBox.Password);
                }
                catch (Exception ex)
                {
                    switch (ex.HResult)
                    {
                        case WrongPassword:
                            rootPage.NotifyUser("Document is password-protected and password is incorrect.", NotifyType.ErrorMessage);
                            break;

                        case GenericFail:
                            rootPage.NotifyUser("Document is not a valid PDF.", NotifyType.ErrorMessage);
                            break;

                        default:
                            // File I/O errors are reported as exceptions.
                            rootPage.NotifyUser(ex.Message, NotifyType.ErrorMessage);
                            break;
                    }
                }

                if (pdfDocument != null)
                {
                    RenderingPanel.Visibility = Visibility.Visible;
                    if (pdfDocument.IsPasswordProtected)
                    {
                        rootPage.NotifyUser("Document is password protected.", NotifyType.StatusMessage);
                    }
                    else
                    {
                        rootPage.NotifyUser("Document is not password protected.", NotifyType.StatusMessage);
                    }
                    PageCountText.Text = pdfDocument.PageCount.ToString();
                }
                ProgressControl.Visibility = Visibility.Collapsed;
            }
            LoadButton.IsEnabled = true;
        }

        private async void ViewPage()
        {
            rootPage.NotifyUser("", NotifyType.StatusMessage);

            uint pageNumber;
            if (!uint.TryParse(PageNumberBox.Text, out pageNumber) || (pageNumber < 1) || (pageNumber > pdfDocument.PageCount))
            {
                rootPage.NotifyUser("Invalid page number.", NotifyType.ErrorMessage);
                return;
            }

            Output.Source = null;
            ProgressControl.Visibility = Visibility.Visible;

            // Convert from 1-based page number to 0-based page index.
            uint pageIndex = pageNumber - 1;

            using (PdfPage page = pdfDocument.GetPage(pageIndex))
            {
                var stream = new InMemoryRandomAccessStream();
                switch (Options.SelectedIndex)
                {
                    // View actual size.
                    case 0:
                        await page.RenderToStreamAsync(stream);
                        break;

                    // View half size on beige background.
                    case 1:
                        var options1 = new PdfPageRenderOptions();
                        options1.BackgroundColor = Windows.UI.Colors.Beige;
                        options1.DestinationHeight = (uint)(page.Size.Height / 2);
                        options1.DestinationWidth = (uint)(page.Size.Width / 2);
                        await page.RenderToStreamAsync(stream, options1);
                        break;

                    // Crop to center.
                    case 2:
                        var options2 = new PdfPageRenderOptions();
                        var rect = page.Dimensions.TrimBox;
                        options2.SourceRect = new Rect(rect.X + rect.Width / 4, rect.Y + rect.Height / 4, rect.Width / 2, rect.Height / 2);
                        await page.RenderToStreamAsync(stream, options2);
                        break;
                }
                BitmapImage src = new BitmapImage();
                Output.Source = src;
                await src.SetSourceAsync(stream);
            }
            ProgressControl.Visibility = Visibility.Collapsed;
        }
    }
}
