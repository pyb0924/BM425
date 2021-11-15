using FellowOakDicom;
using Microsoft.Win32;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace MyPACSViewer
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        public MainWindow()
        {
            InitializeComponent();
 
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

        private void OpenFileMenu_Click(object sender, RoutedEventArgs e)
        {
            OpenFileDialog openFileDialog =new ();
            openFileDialog.Title = "Open DICOM File";
            openFileDialog.Filter = "DICOM Files|*.dcm";
            openFileDialog.FileName = string.Empty;
            openFileDialog.FilterIndex = 1;
            openFileDialog.Multiselect = false;
            openFileDialog.RestoreDirectory = true;
            openFileDialog.DefaultExt = "dcm";
            if (openFileDialog.ShowDialog() == false)
            {
                return;
            }
            DicomFile dcmFile = DicomFile.Open(openFileDialog.FileName);
            DicomDataset dcmDataSet = dcmFile.Dataset;

            List<FileNodeItem> treeList = new();
            FileNodeItem patientNode = new() { Name = dcmDataSet.GetString(DicomTag.PatientName), Icon = FileNodeItem.PatientIcon };
            FileNodeItem studyNode = new() { Name = dcmDataSet.GetString(DicomTag.Modality), Icon = FileNodeItem.StudyIcon };
            FileNodeItem seriesNode = new() { Name = dcmDataSet.GetString(DicomTag.SeriesDescription), Icon = FileNodeItem.SeriesIcon };
            FileNodeItem instanceNode = new() { Name = openFileDialog.FileName, Icon = FileNodeItem.InstanceIcon };

            seriesNode.Children.Add(instanceNode);
            studyNode.Children.Add(seriesNode);
            patientNode.Children.Add(studyNode);

            treeList.Add(patientNode);
            fileExplorer.ItemsSource = treeList;
        }

        private void OpenFolderMenu_Click(object sender, RoutedEventArgs e)
        {
           
        }
    }
}
