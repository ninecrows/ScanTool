using MongoDB.Bson;
using MongoDB.Driver;
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

namespace ScanTool
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        public MainWindow()
        {
            InitializeComponent();

            MongoClient dbClient = new MongoClient("mongodb://127.0.0.1:27017");
            IMongoDatabase db = dbClient.GetDatabase("FileInformation");

            // Get the volumes document. We'll want to update it as we go.
            var volumesList = db.GetCollection<VolumeInformation>("volumes");

            VolumeInformation v = new VolumeInformation("c:\\");
            String vsn = v.SerialNumber();
            String vv = v.VolumeName();
            String fstype = v.FileSystemName();

            volumesList.InsertOne(v);

            String fid = Helpers.GetFileId("c:\\windows-version.txt");
            String[] guids = VolumeInformation.GetVolumeGuids();
            foreach (String item in guids)
            {
                String[] options = VolumeInformation.GetMountPoints(item);
            }
        }
    }
}
