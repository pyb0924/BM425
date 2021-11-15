using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace MyPACSViewer
{
    internal class FileNodeItem
    {
        public string Name { get; set; }
        public string Icon { get; set; }
        public static readonly string PatientIcon = "/Resources/patient.png";
        public static readonly string StudyIcon = "/Resources/study.png";
        public static readonly string SeriesIcon = "/Resources/series.png";
        public static readonly string InstanceIcon = "/Resources/instance.png";

        public List<FileNodeItem> Children { get; set; }

        public FileNodeItem()
        {
            Children = new List<FileNodeItem>();
        }
    }
}
