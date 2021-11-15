
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Forms;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

using FellowOakDicom;
using System.IO;

namespace MyPACSViewer
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        private readonly Dictionary<string, FileNodeItem> DicomDict;
        private int FileCount;

        public MainWindow()
        {
            InitializeComponent();
            DicomDict = new();
        }

        
        private void OpenFileMenu_Click(object sender, RoutedEventArgs e)
        {
            OpenFileDialog openFileDialog = new();
            openFileDialog.Title = "Open DICOM File";
            openFileDialog.Filter = "DICOM Files|*.dcm";
            openFileDialog.FileName = string.Empty;
            openFileDialog.FilterIndex = 1;
            openFileDialog.Multiselect = false;
            openFileDialog.RestoreDirectory = true;
            openFileDialog.DefaultExt = "dcm";
            if (openFileDialog.ShowDialog() != System.Windows.Forms.DialogResult.OK)
            {
                StatusBarText.Text = "Open File Failed!";
                return;
            }
            DicomDict.Clear();

            DicomFile dcmFile = DicomFile.Open(openFileDialog.FileName);
            DicomDataset dcmDataSet = dcmFile.Dataset;

            FileNodeItem patientNode = new(dcmDataSet.GetString(DicomTag.PatientName), FileNodeItem.PatientIcon);
            FileNodeItem studyNode = new(dcmDataSet.GetString(DicomTag.Modality), FileNodeItem.StudyIcon);
            FileNodeItem seriesNode = new(dcmDataSet.GetString(DicomTag.SeriesDescription), FileNodeItem.SeriesIcon);
            FileNodeItem imageNode = new(openFileDialog.FileName, FileNodeItem.InstanceIcon, openFileDialog.FileName);

            DicomDict.Add(dcmDataSet.GetString(DicomTag.PatientID), patientNode);
            patientNode.Children.Add(dcmDataSet.GetString(DicomTag.StudyInstanceUID), studyNode);
            studyNode.Children.Add(dcmDataSet.GetString(DicomTag.SeriesInstanceUID), seriesNode);
            seriesNode.Children.Add(dcmDataSet.GetString(DicomTag.SOPInstanceUID), imageNode);

            fileExplorer.ItemsSource = DicomDict.Values;
            FileCount = 1;
            StatusBarText.Text = "Open File Successfully!";
        }

        private void OpenFolderMenu_Click(object sender, RoutedEventArgs e)
        {
            FolderBrowserDialog openFolderDialog = new();
            if (openFolderDialog.ShowDialog() != System.Windows.Forms.DialogResult.OK)
            {
                StatusBarText.Text = "Open Folder Failed!";
                return;
            }
            DirectoryInfo dir = new(openFolderDialog.SelectedPath);
            var files = dir.GetFiles("*.dcm", SearchOption.AllDirectories);

            DicomDict.Clear();
            string tmp;
            DicomFile dcmFile;
            DicomDataset dcmDataSet;
            FileNodeItem patientNode, studyNode, seriesNode, imageNode;
            foreach (FileInfo file in files)
            {
                dcmFile = DicomFile.Open(file.FullName);
                dcmDataSet = dcmFile.Dataset;
                tmp = dcmDataSet.GetString(DicomTag.PatientID);
                if (DicomDict.ContainsKey(tmp))
                {
                    patientNode = DicomDict[tmp];
                }
                else
                {
                    patientNode = new(dcmDataSet.GetString(DicomTag.PatientName), FileNodeItem.PatientIcon);
                    DicomDict.Add(tmp, patientNode);
                }

                tmp = dcmDataSet.GetString(DicomTag.StudyInstanceUID);
                if (patientNode.Children.ContainsKey(tmp))
                {
                    studyNode = patientNode.Children[tmp];
                }
                else
                {
                    studyNode = new(dcmDataSet.GetString(DicomTag.Modality), FileNodeItem.StudyIcon);
                    patientNode.Children.Add(tmp, studyNode);
                }

                tmp = dcmDataSet.GetString(DicomTag.SeriesInstanceUID);
                if (studyNode.Children.ContainsKey(tmp))
                {
                    seriesNode = studyNode.Children[tmp];
                }
                else
                {
                    seriesNode = new(dcmDataSet.GetString(DicomTag.SeriesDescription), FileNodeItem.SeriesIcon);
                    studyNode.Children.Add(tmp, seriesNode);
                }

                tmp = dcmDataSet.GetString(DicomTag.SOPInstanceUID);
                if (seriesNode.Children.ContainsKey(tmp))
                {
                    imageNode = seriesNode.Children[tmp];
                }
                else
                {
                    imageNode = new(file.Name, FileNodeItem.InstanceIcon, file.FullName);
                    seriesNode.Children.Add(tmp, imageNode);
                }
            }
            fileExplorer.ItemsSource = new List<FileNodeItem>(DicomDict.Values);
            FileCount = files.Length;
            StatusBarText.Text = "Open Folder Successfully! Total: " + FileCount.ToString() + " Files";

        }

        private void ExportMenu_Click(object sender, RoutedEventArgs e)
        {

        }

        private void ExitMenu_Click(object sender, RoutedEventArgs e)
        {
            System.Windows.Application.Current.MainWindow.Close();
        }


        private void RevertBtn_Click(object sender, RoutedEventArgs e)
        {
            
        }

        private void Rotate_verticalBtn_Click(object sender, RoutedEventArgs e)
        {

        }

        private void Rotate_horizontalBtn_Click(object sender, RoutedEventArgs e)
        {

        }

    }
}
