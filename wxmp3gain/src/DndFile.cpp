/*
 * This file is part of the wxMP3gain and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#include "DndFile.h"

DndFile::DndFile(wxListCtrl *owner, ArrayOfFiles *lstFilesData) : lstFilesData(lstFilesData)
{
    m_owner = owner;
}

DndFile::~DndFile()
{
    //dtor
}

bool DndFile::OnDropFiles(wxCoord x, wxCoord y, const wxArrayString& filenames)
{
    wxArrayString files;

    // Workaround to work DragAndDrop with LXDE
    wxArrayString tempFilenames;
    if(filenames.GetCount() == 1)
    {
        wxStringTokenizer strToken(filenames[0]);
        while (strToken.HasMoreTokens())
        {
            tempFilenames.Add(strToken.GetNextToken());
        }
    }
    else
    {
        tempFilenames = filenames;
    }

    // Check if is a directory or a file
    for (size_t n = 0; n < tempFilenames.GetCount(); n++)
    {
        wxString filename = tempFilenames[n];

        // Workaround to work with "file://" string filename in some Linux distros
        if(filename.Contains(_T("//")))
        {
            filename = filename.Mid(filename.Find(_T("//"))+2);
        }

        if (wxFileName::DirExists(filename))
            InsertFileListDir(filename);
        else
            files.Add(filename);
    }
    InsertFileList(files);

    return true;
}

void DndFile::InsertFileList(const wxArrayString& filenames)
{
    wxFileName file;
    size_t nFiles = filenames.GetCount();

    // Add files in wxListCtrl
    for (size_t n = 0; n < nFiles; n++)
    {
        file.SetFullName(filenames[n]);

        if (CheckValidExtension(file))
        {
            // Don't insert repeated filenames
            bool repeated = false;
            for (int i = 0; i < m_owner->GetItemCount(); i++)
            {
                FileInfo& fileInfo = lstFilesData->Item(i);
                wxFileName filenameInput = fileInfo.getFileName();
                if (filenameInput.GetFullPath() == filenames[n])
                {
                    repeated = true;
                }
            }
            if (!repeated)
            {
                m_owner->InsertItem(m_owner->GetItemCount(), file.GetFullName());
                lstFilesData->Add(new FileInfo(filenames[n]));
            }
        }
    }
}

void DndFile::InsertFileListDir(const wxString& dirname)
{
    wxArrayString files;
    wxDir::GetAllFiles(dirname, &files);

    InsertFileList(files);
}

bool DndFile::CheckValidExtension(const wxFileName& file)
{
    wxStringTokenizer strToken(APP_OPEN_EXT, _T(";"));
    while (strToken.HasMoreTokens())
    {
        wxString token = strToken.GetNextToken();

        if (file.GetExt().CmpNoCase(token) == 0)
            return true;
    }
    return false;
}
