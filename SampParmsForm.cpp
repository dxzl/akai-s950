//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "SampParmsForm.h"
#include "ListBoxForm.h"
#include "S9Utils.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFormEditSampParms *FormEditSampParms;
using namespace S9Utils;
//---------------------------------------------------------------------------
__fastcall TFormEditSampParms::TFormEditSampParms(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TFormEditSampParms::FormClose(TObject *Sender, TCloseAction &Action)
{
	Release();
}
//---------------------------------------------------------------------------
void __fastcall TFormEditSampParms::FormDestroy(TObject *Sender)
{
	if (m_slSampleData) delete m_slSampleData;
}
//---------------------------------------------------------------------------
void __fastcall TFormEditSampParms::FormCreate(TObject *Sender)
{
	m_sampIndex = -1;

	// avoid divide by 0 error if user clicks prior to loading parameters!
	TBStart->Max = 0;
	TBEnd->Max = 0;
	TBLoop->Max = 0;

	m_iSource = SOURCE_UNDEFINED;

	MenuSave->Enabled = false;
	ButtonSendOrSave->Enabled = false;
	ButtonSendOrSave->Caption = SOS_SEND;

	m_slSampleData = NULL;
	ButtonRefreshSamplesListClick(NULL);
}
//---------------------------------------------------------------------------
void __fastcall TFormEditSampParms::FormShow(TObject *Sender)
{
  // Drop the program-name list down
  if (ComboBoxSampNames->Items->Count && !ComboBoxSampNames->DroppedDown)
    ComboBoxSampNames->DroppedDown = true;
}
//---------------------------------------------------------------------------
void __fastcall TFormEditSampParms::ButtonCloseClick(TObject *Sender)
{
	Close();
}
//---------------------------------------------------------------------------
void __fastcall TFormEditSampParms::ButtonRefreshSamplesListClick(TObject *Sender)

{
	// refresh the catalog
	int iErrorCode = RefreshCatalog();
	if (iErrorCode < 0 && iErrorCode != -1)
		ShowMessage("Error: unable to allocate m_slSampleData/ProgramData! (code=" + String(iErrorCode) + ")");

	RefreshSamplesInComboBox(0);
}
//---------------------------------------------------------------------------
// return 0 if success
int __fastcall TFormEditSampParms::RefreshCatalog(void)
{
	try
	{

		int iErrorCode = FormMain->GetCatalog();

		// Invoke property getters in FormMain that create string-lists.
		// (MUST delete on destroy or prior to reinvoking!)
		if (m_slSampleData)
			delete m_slSampleData;
		m_slSampleData = FormMain->SampleData;
		if (!m_slSampleData)
			return -2;

		// put error-code return here so we can load test SampleData (above)
		if (iErrorCode < 0)
		{
			if (iErrorCode == -2)
				ShowMessage("Transmit timeout requesting catalog!");
			else if (iErrorCode == -3)
				ShowMessage("Timeout receiving catalog!");
			else if (iErrorCode == -4)
				ShowMessage("Incorrect catalog received!");
			else
				ShowMessage("Not able to get catalog, check power and cables... (code=" + String(iErrorCode) + ")");
			return -1;
		}

		return 0;
	}
	catch(...)
	{
		return -100;
	}
}
//---------------------------------------------------------------------------
// Returns 0 if success, negative if fail, -1 if error message already shown
int __fastcall TFormEditSampParms::RefreshSamplesInComboBox(int iSampIdx)
{
	try
	{
		m_sampIndex = -1;

		if (!m_slSampleData)
			return -2;

		ComboBoxSampNames->Clear();

		// add samples to combo box with index in the objects list
		ComboBoxSampNames->Items->Assign(m_slSampleData);

		int count = ComboBoxSampNames->Items->Count;

		if (count && iSampIdx >= 0 && iSampIdx < count)
		{
			ComboBoxSampNames->ItemIndex = iSampIdx;
			ComboBoxSampNamesSelect(NULL); // trigger an OnSelect event (load sample)
		}
		else
			ComboBoxSampNames->Text = "<No Samples!>";

		return 0;
	}
	catch(...)
	{
		ShowMessage("Unexpected error, RefreshSamplesInComboBox(), please tell author!");
		return -1;
	}
}
//---------------------------------------------------------------------------
// Takes info in GUI, including possible new sample-name, and sends it
int __fastcall TFormEditSampParms::SendParmsToMachine(int iSampIdx)
{
	// Send samp parms to machine
	try
	{
		MenuSave->Enabled = false;
		ButtonSendOrSave->Enabled = false;

		int iErrorCode = ParmsFromGui();
		if (iErrorCode < 0)
		{
			if (iErrorCode != -1)
			{
				ShowMessage("Error in ParmsFromGui(), (code=" +
																								String(iErrorCode) + ")");
				return -1;
			}
			return -2;
		}

		iErrorCode = ParmsToArray(iSampIdx);
		if (iErrorCode < 0)
		{
			if (iErrorCode != -1)
			{
				ShowMessage("Error in ParmsToArray(), (code=" +
																								String(iErrorCode) + ")");
				return -1;
			}
			return -3;
		}

		// transmit sample parameters (after 25ms delay)
		if (!FormMain->comws(PARMSIZ, m_samp_parms, true))
			return -4;

		// better delay a bit...
		FormMain->DelayGpTimer(DELAY_BETWEEN_EACH_PROGRAM_TX);

		// refresh the catalog and warn user of any changes
		iErrorCode = RefreshCatalog();
		if (iErrorCode < 0)
		{
			if (iErrorCode != -1)
				ShowMessage("unable to allocate m_slSampleData/ProgramData! (code=" + String(iErrorCode) + ")");
			return -5;
		}

		// find the index of the new name (should equal iSampIdx, but who knows?)
		int iNewIdx = m_slSampleData->IndexOf(String(m_ps.name));

		if (iNewIdx < 0)
		{
			ShowMessage("can't find new sample-name in current machine-catalog!");
			return -6;
		}

		iErrorCode = RefreshSamplesInComboBox(iNewIdx);
		if (iErrorCode < 0)
		{
			if (iErrorCode != -1)
				ShowMessage("unable to refresh samples in ComboBox! (code=" + String(iErrorCode) + ")");
			return -7;
		}

		if (!ComboBoxSampNames->Items->Count)
		{
			ShowMessage("no samples on machine!");
			return -8;
		}

		MenuSave->Enabled = true;
		ButtonSendOrSave->Enabled = true;
		return 0;
	}
	catch(...)
	{
		return -100;
	}
}
//---------------------------------------------------------------------------
// Put ps (PSTOR struct) into GUI
int __fastcall TFormEditSampParms::ParmsToGui(int iMode)
{
	// disable event handlers while we set the initial track-bar positions
	TBStart->OnChange = NULL;
	TBEnd->OnChange = NULL;
	TBLoop->OnChange = NULL;
	m_iSource = SOURCE_UNDEFINED;

	try
	{
		try
		{
			MenuSave->Enabled = false;
			ButtonSendOrSave->Enabled = false;
			ButtonSendOrSave->Caption = (iMode == SOURCE_MACHINE) ? SOS_SEND : SOS_SAVE;

			ComboBoxSampNames->Text = String(m_ps.name);
			// Note: 3C/-15 is the same as 2B/+1. If we enter 3C/-15 into the GUI and save it,
			// then read the file back, we will see 2B/+1... same thing!
			EditNominalPitch->Text = S9Utils::KeyToString(m_ps.pitch/16);
			EditFinePitch->Text = String(m_ps.pitch%16);
			EditLoudness->Text = String(m_ps.loudnessoffset);

			if (m_ps.loopmode == 'O')
				RadioGroupReplayMode->ItemIndex = 0;
			else if (m_ps.loopmode == 'L')
				RadioGroupReplayMode->ItemIndex = 1;
			else if (m_ps.loopmode == 'A')
				RadioGroupReplayMode->ItemIndex = 2;
			else
				RadioGroupReplayMode->ItemIndex = -1;

			// I think this must be a flag used internally, it's not in the
			// S950 user interface
			// m_ps.flags bit 0 is crossfade mode, 1=velocity xfade, 0=normal
	//		bool xfade = (m_ps.flags & 1) ? true : false;
	//		if (xfade)
	//			LabelSampleType->Caption = "Sample Type: Velocity Crossfade";
	//		else
	//			LabelSampleType->Caption = "Sample Type: Normal";

			// m_ps.flags bit 1 is waveform mode, 1=reverse, 0=normal
			bool reverse = (m_ps.flags & 2) ? true : false;
			if (reverse)
				RadioGroupWaveForm->ItemIndex = 1; // reverse
			else
				RadioGroupWaveForm->ItemIndex = 0; // normal

			LabelSampleRate->Caption = "Sample Rate: " + String(m_ps.freq) + " Hz";
			LabelSampleRate->Tag = m_ps.freq; // save the raw # in Tag
			LabelSampleLength->Caption = "Sample Length: " + String(m_ps.totalct);
			LabelSampleLength->Tag = m_ps.totalct; // save the raw # in Tag
      UInt32 iPeriod = 1.0e9 / (double)m_ps.freq; // in nanoseconds
 			LabelSampleTime->Caption = "Time (mS): " + String(m_ps.endpoint*iPeriod/1.0e6);

			// Set up the trackbars and associated edit-controls
			TBStart->Min = 0;
			TBEnd->Min = 0;
			TBLoop->Min = 0;
			UpDownStart->Min = 0;
			UpDownEnd->Min = 0;
			UpDownLoop->Min = 0;

			TBStart->Max = m_ps.totalct;
			TBEnd->Max = m_ps.totalct;
			TBLoop->Max = m_ps.totalct;
			UpDownStart->Max = m_ps.totalct;
			UpDownEnd->Max = m_ps.totalct;
			UpDownLoop->Max = m_ps.totalct;

			TBStart->Position = m_ps.startpoint;
			UpDownStart->Position = TBStart->Position;

			TBEnd->Position = m_ps.endpoint;
			UpDownEnd->Position = TBEnd->Position;

			int iTemp = m_ps.endpoint-m_ps.looplen;
			if (iTemp < (int)m_ps.startpoint)
				iTemp = m_ps.startpoint;
			TBLoop->Position = iTemp;
			UpDownLoop->Position = iTemp;

			m_iSource = iMode; // define SOURCE_FILE or SOURCE_MACHINE
			MenuSave->Enabled = true;
			ButtonSendOrSave->Enabled = true;

			ComputeTimes();
		}
		__finally
		{
			// re-enable event handlers
			TBStart->OnChange = TBStartChange;
			TBEnd->OnChange = TBEndChange;
			TBLoop->OnChange = TBLoopChange;
		}

		return 0;
	}
	catch(...)
	{
		return -100;
	}
}
//---------------------------------------------------------------------------
// Put GUI into ps (PSTOR struct) [do not clear unused fields of PSTOR struct!]
int __fastcall TFormEditSampParms::ParmsFromGui(void)
{
	try
	{
		AnsiString sSampName = ComboBoxSampNames->Text.TrimRight();
		ComboBoxSampNames->Text = sSampName;
		if (!NameOk(sSampName))
		{
			ShowMessage("Sample name must be 1-10 ASCII characters!");
			ComboBoxSampNames->SetFocus();
			return -1;
		}

		strcpy(&m_ps.name[0], sSampName.c_str());

		// 12 notes per octave * 16 fine steps per semitone = 192 fine steps per octave
		// From octave -2C to octave +3C is 192*5 = 960 fine steps (middle C)
		// 0 = -2C, 24 is +0C, 60 is +3C, 127 is +8G
		//
		// Note: 3C/-15 is the same as 2B/+1. If we enter 3C/-15 into the GUI and save it,
		// then read the file back, we will see 2B/+1... same thing!
		int iTemp = StringToKey(EditNominalPitch->Text);
		if (iTemp < 0)
		{
			ShowMessage("Nominal pitch is of the form -2Eb, +8G, C#, or midi key 0-127!");
			EditNominalPitch->SetFocus();
			return -1;
		}
		m_ps.pitch = iTemp * 16; // key (0-127) * 16 steps per semitone

		iTemp = EditFinePitch->Text.ToIntDef(-1000);
		if (iTemp > 15 || iTemp < -15)
		{
			ShowMessage("Fine pitch is +/-15 in 1/16 semitone steps!");
			EditFinePitch->SetFocus();
			return -1;
		}
		m_ps.pitch += iTemp;

		iTemp = EditLoudness->Text.ToIntDef(-1000);
		if (iTemp > 50 || iTemp < -50)
		{
			ShowMessage("Loudness is +/- 50!");
			EditLoudness->SetFocus();
			return -1;
		}
		m_ps.loudnessoffset = iTemp;

		int iSampleLength = LabelSampleLength->Tag; // (we saved this constant in the Tag property)

		iTemp = TBEnd->Position;
		if (iTemp > iSampleLength || iTemp < 0)
		{
			ShowMessage("Endpoint must be 0 to " + String(iSampleLength) + "!");
			EditEnd->SetFocus();
			return -1;
		}
		m_ps.endpoint = iTemp;

		iTemp = TBStart->Position;
		if (iTemp > (int)m_ps.endpoint || iTemp < 0)
		{
			ShowMessage("Startpoint must be 0 to " + String(m_ps.endpoint) + "!");
			EditStart->SetFocus();
			return -1;
		}
		m_ps.startpoint = iTemp;

		iTemp = TBEnd->Position-TBLoop->Position;
		if (iTemp < 5 || iTemp > iSampleLength)
		{
			ShowMessage("Loop length must be 5 to " + String(iSampleLength-(int)m_ps.startpoint) + "!");
			EditLoop->SetFocus();
			return -1;
		}
		m_ps.looplen = iTemp;


		switch (RadioGroupReplayMode->ItemIndex)
		{
			case 0:
				m_ps.loopmode = 'O';
			break;
			case 1:
				m_ps.loopmode = 'L';
			break;
			case 2:
				m_ps.loopmode = 'A';
			break;
			default:
				m_ps.loopmode = 'O';
			break;
		};

// I think this must be a flag used internally, it's not in the S950 user interface
//		switch (RadioGroupSampleType->ItemIndex)
//		{
//			case 0: // normal
//				m_ps.flags &= ~1;
//			break;
//			case 1: // velocity crossfade
//				m_ps.flags |= 1;
//			break;
//			default:
//				m_ps.flags &= ~1;
//			break;
//		};

		switch (RadioGroupWaveForm->ItemIndex)
		{
			case 0: // normal
				m_ps.flags &= ~2;
			break;
			case 1: // reversed
				m_ps.flags |= 2;
			break;
			default:
				m_ps.flags &= ~2;
			break;
		};

		return 0;
	}
	catch(...)
	{
		return -100;
	}
}
//---------------------------------------------------------------------------
// put ps (PSTOR struct) into m_samp_parms raw transmit array
int __fastcall TFormEditSampParms::ParmsToArray(UInt16 index)
{
	try
	{
		//
		// SAMPLE PARAMETERS 129 bytes (do this before encoding header)
		//

		// Clear array to all 0
		memset(m_samp_parms, 0, PARMSIZ);

		m_samp_parms[0] = BEX;
		m_samp_parms[1] = AKAI_ID;
		m_samp_parms[2] = 0; // midi chan
		m_samp_parms[3] = SPRM;
		m_samp_parms[4] = S900_ID;
		m_samp_parms[5] = (Byte)(index & 0x7f);
		m_samp_parms[6] = (Byte)((index>>7) & 0x7f); // reserved

		// copy ASCII sample name, pad with blanks and terminate
    AsciiStrEncode(&m_samp_parms[7], m_ps.name);

		 // unused bytes
		 // 27-34  DD Undefined
		 // 35-38  DW Undefined

		// number of sample words
		encodeDD(m_ps.totalct, &m_samp_parms[39]); // 8

		// sampling frequency
		encodeDW(m_ps.freq, &m_samp_parms[47]); // 4

		// pitch
		encodeDW(m_ps.pitch, &m_samp_parms[51]); // 4

													// loudness offset
		encodeDW(m_ps.loudnessoffset, &m_samp_parms[55]); // 4

		// replay mode 'A', 'L' or 'O' (alternating, looping or one-shot)
		// (samp_hedr[18] also has sample's loop mode: 0=looping, 1=alternating
		// or one-shot if loop-length < 5)
		encodeDB(m_ps.loopmode, &m_samp_parms[59]); // 2

		encodeDB(0, &m_samp_parms[61]); // reserved 2

		// end point relative to start of sample
		encodeDD(m_ps.endpoint, &m_samp_parms[63]); // 8

		// loop start point
		encodeDD(m_ps.startpoint, &m_samp_parms[71]); // 8

		// limit loop length (needed below when setting samp_hedr[15])
		if (m_ps.looplen > m_ps.endpoint)
			m_ps.looplen = m_ps.endpoint;

		// length of looping or alternating part (default is 45 for TONE sample)
		encodeDD(m_ps.looplen, &m_samp_parms[79]); // 8

		encodeDW(0, &m_samp_parms[87]); // reserved 4

		// 91,92 DB type of sample 0=normal, 255=velocity crossfade
		Byte c_temp = (Byte)((m_ps.flags & (Byte)1) ? 255 : 0);
		encodeDB(c_temp, &m_samp_parms[91]);

		// 93,94 DB waveform type 'N'=normal, 'R'=reversed
		c_temp = (m_ps.flags & (Byte)2) ? 'R' : 'N';
		encodeDB(c_temp, &m_samp_parms[93]); // 2

		// unused bytes
		// 95-126  4 DDs Undefined 32

		// checksum is exclusive or of 7-126 (120 bytes)
		// and the value is put in index 127
		compute_checksum(7, 127, m_samp_parms);

		m_samp_parms[128] = EEX;

		return 0;
	}
	catch(...)
	{
		return -100;
	}
}
//---------------------------------------------------------------------------
// put m_samp_parms raw transmit array into ps (PSTOR struct)
int __fastcall TFormEditSampParms::ParmsFromArray(void)
{
	try
	{
		// FROM AKAI EXCLUSIVE SAMPLE PARAMETERS... (do this before decoding header)

		// clear spare-byte fields
		for (int ii = 0; ii < PSTOR_SPARE_COUNT; ii++)
			m_ps.spares[ii] = (Byte)0;
		m_ps.spareint1 = 0;
		m_ps.spareint2 = (Byte)0;
		m_ps.sparechar1 = (Byte)0;
		m_ps.sparechar2 = (Byte)0;

		// ASCII sample name
    AsciiStrDecode(m_ps.name, &m_samp_parms[7]);

		// undefined
		// m_ps.undef_dd = decodeDD((Byte*)&m_samp_parms[27]); // 8

		// undefined
		// m_ps.undef_dw = decodeDW((Byte*)&m_samp_parms[35]); // 4

		// number of words in sample (for velocity-crossfade it's the sum of soft and loud parts)
		m_ps.totalct = decodeDD(&m_samp_parms[39]); // 8

		// original sample rate in Hz
		m_ps.freq = decodeDW(&m_samp_parms[47]); // 4

		// nominal pitch in 1/16 semitone, C3=960
		m_ps.pitch = decodeDW(&m_samp_parms[51]); // 4

		// loudness offset (signed) nominal pitch in 1/16 semitone, C3=960
		m_ps.loudnessoffset = decodeDW(&m_samp_parms[55]); // 4

		// replay mode 'A', 'L' or 'O' (alternating, looping or one-shot)
		// (samp_hedr[18] also has sample's loop mode: 0=looping, 1=alternating
		// or one-shot if loop-length < 5)
		m_ps.loopmode = decodeDB(&m_samp_parms[59]); // 2

		// 61,62 DB reserved
		//m_ps.reserved = decodeDB(&m_samp_parms[61]); // 2

		// 63-70 DD end point relative to start of sample (1800 default for TONE sample)
		m_ps.endpoint = decodeDD(&m_samp_parms[63]);

		// 71-78 DD first replay point relative to start of sample (default is 0)
		m_ps.startpoint = decodeDD(&m_samp_parms[71]);

		// 79-86 DD length of looping or alternating part
		m_ps.looplen = decodeDD(&m_samp_parms[79]);

		// 87-90 DW reserved

		// 91,92 DB type of sample 0=normal, 255=velocity crossfade
		bool xfade = (decodeDB(&m_samp_parms[91]) == (Byte)255); // 2

		// 93,94 DB sample waveform 'N'=normal, 'R'=reversed
		bool reversed = (decodeDB(&m_samp_parms[93]) == 'R'); // 2

		m_ps.flags = 0;
		if (xfade)
			m_ps.flags |= (Byte)1;
		if (reversed)
			m_ps.flags |= (Byte)2;
		//if (???)
		//  m_ps.flags |= (Byte)4;
		//if (???)
		//  m_ps.flags |= (Byte)8;
		//if (???)
		//  m_ps.flags |= (Byte)16;
		//if (???)
		//  m_ps.flags |= (Byte)32;
		//if (???)
		//  m_ps.flags |= (Byte)64;
		//if (???)
		//  m_ps.flags |= (Byte)128;

		// 95-126 (4 DDs) undefined

		// FROM SAMPLE HEADER... (NOT USED HERE!)
		// These fields are written when a .aki file is created (in MainForm.cpp).
		// We don't have access to them when simply editing the sample parameters
		// obtained from the machine. SO DO NOT USE!
		m_ps.bits_per_word = 0;
		m_ps.period = 0;

		return 0;
	}
	catch(...)
	{
		return -100;
	}
}
//---------------------------------------------------------------------------
// Save the file previously opened file or send to machine
void __fastcall TFormEditSampParms::MenuSaveClick(TObject *Sender)
{
	int iErrorCode;

	if (m_iSource == SOURCE_FILE)
	{
		iErrorCode = SaveSampParmsToFile();

		if (iErrorCode < 0)
		{
			if (iErrorCode != -1)
				ShowMessage("Unable to save file: \"" +
							m_sFilePath + "\" SaveSampParmsToFile(), (code=" +
																								String(iErrorCode) + ")");
		}
	}
	else if (m_iSource == SOURCE_MACHINE)
	{
		if (m_sampIndex >= 0 && m_sampIndex < ComboBoxSampNames->Items->Count)
		{
			iErrorCode = SendParmsToMachine(m_sampIndex);

			if (iErrorCode < 0)
			{
				if (iErrorCode != -1)
					ShowMessage("Error in SendParmsToMachine(), (code=" +
																									String(iErrorCode) + ")");
				iErrorCode = RefreshSamplesInComboBox(0);
				if (iErrorCode < 0)
				{
					if (iErrorCode != -1)
						ShowMessage("unable to refresh samples in ComboBox! (code=" + String(iErrorCode) + ")");
					return;
				}
			}
		}
	}
}
//---------------------------------------------------------------------------
// Save sample header to previously opened .aki file
// returns negative error-code if fail, 0 if success
int __fastcall TFormEditSampParms::SaveSampParmsToFile(void)
{
	try
	{
		int iFileHandle = 0;

		try
		{
			String s = "Update sample parameters for file?\n(" + m_sFilePath + ")";
			int button = MessageBox(Handle, s.w_str(), L"Update .aki file parameters",
								MB_ICONQUESTION + MB_YESNO + MB_DEFBUTTON1);

			if (button == IDNO)
				return -1;

			if (!FileExists(m_sFilePath))
				return -2;

			iFileHandle = FileOpen(m_sFilePath, fmShareDenyNone | fmOpenReadWrite);

			if (iFileHandle == 0)
				return -3;

			// .aki file
			UInt32 iMagicNumber;
			int byteCount = FileRead(iFileHandle, &iMagicNumber, UINT32SIZE);
			if (byteCount < UINT32SIZE)
				return -4;
			if (iMagicNumber != MAGIC_NUM_AKI)
				return -5;

			int iErrorCode = ParmsFromGui();
			if (iErrorCode < 0)
			{
				if (iErrorCode != -1)
					ShowMessage("Error in ParmsFromGui(), (code=" +
																									String(iErrorCode) + ")");
				return -1;
			}

			byteCount = FileWrite(iFileHandle, &m_ps, AKI_FILE_HEADER_SIZE); // write 72 bytes
			if (byteCount < (int)AKI_FILE_HEADER_SIZE)
				return -6;
		}
		__finally
		{
			if (iFileHandle) FileClose(iFileHandle);
		}
		return 0;
	}
	catch(...)
	{
		return -100;
	}
}
//---------------------------------------------------------------------------
void __fastcall TFormEditSampParms::MenuLoadFromAkiFileClick(TObject *Sender)
{
		int iErrorCode = LoadSampParmsFromFile();

		if (iErrorCode < 0)
		{
			if (iErrorCode != -1)
				ShowMessage("Problem loading file (code=" + String(iErrorCode) + ")");
		}
}
//---------------------------------------------------------------------------
// Load sample header from .aki file
// returns negative error-code if fail, 0 if success
int __fastcall TFormEditSampParms::LoadSampParmsFromFile(void)
{
	OpenDialog1->Title = "Load sample parameters from .aki file for editing";
	OpenDialog1->DefaultExt = "aki";
	OpenDialog1->Filter = "Sample parameters file (*.aki)|*.aki|" "All files (*.*)|*.*";
	OpenDialog1->FilterIndex = 1; // start the dialog showing .aki files
	OpenDialog1->Options.Clear();
	OpenDialog1->Options << ofHideReadOnly << ofPathMustExist <<
									ofFileMustExist << ofEnableSizing;

	if (!OpenDialog1->Execute())
		return -1; // Cancel

	String sFilePath = OpenDialog1->FileName;
	int iFileHandle = 0;

	try
	{
		try
		{
			iFileHandle = FileOpen(sFilePath, fmShareDenyNone | fmOpenRead);

			if (iFileHandle == 0)
				return -2;

			MenuSave->Enabled = false;
			ButtonSendOrSave->Enabled = false;

			// .aki file
			UInt32 iMagicNumber;
			int bytesRead = FileRead(iFileHandle, &iMagicNumber, UINT32SIZE);
			if (bytesRead < UINT32SIZE)
				return -3;
			if (iMagicNumber != MAGIC_NUM_AKI)
				return -4;
			bytesRead = FileRead(iFileHandle, &m_ps, AKI_FILE_HEADER_SIZE); // read 72 bytes
			if (bytesRead < (int)AKI_FILE_HEADER_SIZE)
				return -5;
			if (ParmsToGui(SOURCE_FILE) < 0)
				return -6;
		}
		__finally
		{
			if (iFileHandle) FileClose(iFileHandle);
		}

		m_sFilePath = sFilePath; // set global path

		return 0;
	}
	catch(...)
	{
		return -100;
	}
}
//---------------------------------------------------------------------------
// Get samp parms from machine
void __fastcall TFormEditSampParms::ComboBoxSampNamesSelect(TObject *Sender)
{
	// here, the user has selected a sample from the drop-down list
	// we keep the index reported by the catalog in the Combo box's Objects property
	int idx = (int)ComboBoxSampNames->Items->Objects[ComboBoxSampNames->ItemIndex];
	int iErrorCode = LoadSampParmsFromMachine(idx);
	if (iErrorCode < 0)
	{
		if (iErrorCode != -1)
			ShowMessage("Error in LoadProgramFromMachine(), (code=" +
																							String(iErrorCode) + ")");
	}
}
//---------------------------------------------------------------------------
// Get samp parms from machine
//---------------------------------------------------------------------------
// populate m_samp_parms array
// returns -1 if error message displayed, 0 if success
int __fastcall TFormEditSampParms::LoadSampParmsFromMachine(int iSampIdx)
{
	try
	{
		m_sampIndex = -1; // invalidate current sample

		MenuSave->Enabled = false;
		ButtonSendOrSave->Enabled = false;

		// get the samp parms into TempArray
		int iErrorCode = FormMain->LoadSampParmsToTempArray(iSampIdx);
		if (iErrorCode < 0)
		{
			switch(iErrorCode)
			{
				case -2:
						ShowMessage("transmit timeout!");
				break;
				case -3:
						ShowMessage("timeout receiving sample parameters!");
				break;
				case -4:
						ShowMessage("received wrong bytecount for sample parameters!");
				break;
				case -5:
						ShowMessage("bad checksum for sample parameters!");
				break;
				case -6:
					ShowMessage("exception in LoadSampParmsToTempArray()!");
				break;
				default:
				break;
			};
			return -1; // could not get sample parms
		}

		// copy m_temp_array into m_samp_parms
		memcpy(m_samp_parms, FormMain->TempArray, PARMSIZ);

		iErrorCode = ParmsFromArray();
		if (iErrorCode < 0)
		{
			if (iErrorCode != -1)
				ShowMessage("Could not convert parameters from TempArray! (code=" + String(iErrorCode) + ")");
			return -1;
		}

		iErrorCode = ParmsToGui(SOURCE_MACHINE);
		if (iErrorCode < 0)
		{
			if (iErrorCode != -1)
				ShowMessage("Could not convert parameters to Gui! (code=" + String(iErrorCode) + ")");
			return -1;
		}

		// set global var to use in SendSampParmsToMachine()
		m_sampIndex = iSampIdx;

		return 0;
	}
	catch(...)
	{
		return -2;
	}
}
//---------------------------------------------------------------------------
// We set the Min/Max of the TUpDown and TTrackBar controls when new sample
// parameters are displayed via ParmsToGui()
//---------------------------------------------------------------------------
void __fastcall TFormEditSampParms::TBStartChange(TObject *Sender)
{
	if (TBStart->Max == 0) return;

	DisableEvents();

	int iTemp = TBStart->Position;
	UpDownStart->Position = iTemp;
	if (iTemp > TBEnd->Position)
	{
		TBEnd->Position = iTemp;
		UpDownEnd->Position = iTemp;
	}
	if (iTemp > TBLoop->Position)
	{
		TBLoop->Position = iTemp;
		UpDownLoop->Position = iTemp;
	}
	ComputeTimes();

	EnableEvents();
}
//---------------------------------------------------------------------------
void __fastcall TFormEditSampParms::UpDownStartChangingEx(TObject *Sender, bool &AllowChange,
					int NewValue, TUpDownDirection Direction)
{
	if (TBStart->Max == 0) return;

	DisableEvents();

	TBStart->Position = NewValue;
	if (NewValue > TBEnd->Position)
	{
		TBEnd->Position = NewValue;
		UpDownEnd->Position = NewValue;
	}
	if (NewValue > TBLoop->Position)
	{
		TBLoop->Position = NewValue;
		UpDownLoop->Position = NewValue;
	}
	ComputeTimes();
	AllowChange = true;

	EnableEvents();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void __fastcall TFormEditSampParms::TBEndChange(TObject *Sender)
{
	if (TBEnd->Max == 0) return;

	DisableEvents();

	int iTemp = TBEnd->Position;
	UpDownEnd->Position = iTemp;
	if (iTemp < TBStart->Position)
	{
		TBStart->Position = iTemp;
		UpDownStart->Position = iTemp;
	}
	if (iTemp < TBLoop->Position)
	{
		TBLoop->Position = iTemp;
		UpDownLoop->Position = iTemp;
	}
	ComputeTimes();

	EnableEvents();
}
//---------------------------------------------------------------------------
void __fastcall TFormEditSampParms::UpDownEndChangingEx(TObject *Sender, bool &AllowChange,
					int NewValue, TUpDownDirection Direction)
{
	if (TBEnd->Max == 0) return;

	DisableEvents();

	TBEnd->Position = NewValue;
	if (NewValue < TBStart->Position)
	{
		TBStart->Position = NewValue;
		UpDownStart->Position = NewValue;
	}
	if (NewValue < TBLoop->Position)
	{
		TBLoop->Position = NewValue;
		UpDownLoop->Position = NewValue;
	}
	ComputeTimes();
	AllowChange = true;

	EnableEvents();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void __fastcall TFormEditSampParms::TBLoopChange(TObject *Sender)
{
	if (TBLoop->Max == 0) return;

	DisableEvents();

	int iTemp = TBLoop->Position;
	UpDownLoop->Position = iTemp;
	if (iTemp < TBStart->Position)
	{
		TBStart->Position = iTemp;
		UpDownStart->Position = iTemp;
	}
	if (iTemp > TBEnd->Position)
	{
		TBEnd->Position = iTemp;
		UpDownEnd->Position = iTemp;
	}
	ComputeTimes();

	EnableEvents();
}
//---------------------------------------------------------------------------
void __fastcall TFormEditSampParms::UpDownLoopChangingEx(TObject *Sender, bool &AllowChange,
					int NewValue, TUpDownDirection Direction)
{
	if (TBLoop->Max == 0) return;

	DisableEvents();

	TBLoop->Position = NewValue;
	if (NewValue < TBStart->Position)
	{
		TBStart->Position = NewValue;
		UpDownStart->Position = NewValue;
	}
	if (NewValue > TBEnd->Position)
	{
		TBEnd->Position = NewValue;
		UpDownEnd->Position = NewValue;
	}
	ComputeTimes();
	AllowChange = true;

	EnableEvents();
}
//---------------------------------------------------------------------------
void __fastcall TFormEditSampParms::DisableEvents(void)
{
	TBStart->OnChange = NULL;
	TBEnd->OnChange = NULL;
	TBLoop->OnChange = NULL;

	UpDownStart->OnChangingEx = NULL;
	UpDownEnd->OnChangingEx = NULL;
	UpDownLoop->OnChangingEx = NULL;
}
//---------------------------------------------------------------------------
void __fastcall TFormEditSampParms::EnableEvents(void)
{
	TBStart->OnChange = TBStartChange;
	TBEnd->OnChange = TBEndChange;
	TBLoop->OnChange = TBLoopChange;

	UpDownStart->OnChangingEx = UpDownStartChangingEx;
	UpDownEnd->OnChangingEx = UpDownEndChangingEx;
	UpDownLoop->OnChangingEx = UpDownLoopChangingEx;
}
//---------------------------------------------------------------------------
// display the start, eand and loop in milliseconds, not points
void __fastcall TFormEditSampParms::ComputeTimes(void)
{
	// m_ps.period and m_ps.bits_per_word are 0ed out since we don't have
	// access to the actual sample

	UInt32 iPeriod = 1.0e9 / (double)m_ps.freq; // in nanoseconds

	// times are originally in nanoseconds so convert to milliseconds
//	LabelStartTime->Caption = Format("%-.3g", ARRAYOFCONST((TBStart->Position*iPeriod/1.0e6)));
	LabelStartTime->Caption = String(TBStart->Position*iPeriod/1.0e6);
	LabelEndTime->Caption = String(TBEnd->Position*iPeriod/1.0e6);
	int loopLength = TBEnd->Position-TBLoop->Position;
	LabelLoopLength->Caption = String(loopLength);
	LabelLoopTime->Caption = String(loopLength*iPeriod/1.0e6);
}
//---------------------------------------------------------------------------
void __fastcall TFormEditSampParms::HelpClick(TObject *Sender)
{
	String s =
	"Sample parameters editor for the Akai S900/S950 digital sampler.\n\n"
	"Sample parameters are saved in the header of each .aki sample file."
	" Here, you can edit a .aki file's parameters. Or you can edit the parameters"
	" of a sample that is already on the machine.\n\n"
	" (NOTE: If you edit the .aki file you will still need to drag-drop it"
	" to upload to the sampler; likewise, if you live-edit the paramaters for a"
	" sample on the machine, you will still need to save the sample to a .aki file"
	" on your computer.)\n\n"
	"The sampling frequency and quantization (bits per sample) are fixed and can't"
	" be edited here. You should do your digital sound editing using"
	" a program such as Audacity, then upload the wav to the S950, then use this"
	" editor to change loop-points, Etc., then save the sample and its parameters to"
	" a .aki file. Thereafter, you can drag-drop all of your .aki files and"
	" a .prg file, as a group, onto the main window to upload everything to"
	" the S950.\n\n"
	"There is no playback of a sample as yet, so when you adjust loop-time,"
	" press \"Send to Machine\" to test-play it. Repeat as necessary... :-)\n\n"
	"I should mention that if you set the pitch to +3C with a fine pitch"
	" of -15, it will read back as +2B with a fine pitch of +1, same thing!\n\n"
	"Backup your .aki file before editing its parameters since there is no \"Undo\".";
	ShowMessage(s);
}
//---------------------------------------------------------------------------

