using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using Pinocchio.animation3D;

namespace Pinocchio
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        Game1 game;

        public MainWindow()
        {
            InitializeComponent();

            // TODO test
            Animation3D animation = XmlImporter.Instance.import("../../test.xml");
            XmlExporter.Instance.export("../../output.xml", animation);


            game = new Game1(XNA.Handle, gameScreen.Width, gameScreen.Height);

            this.Closing += new System.ComponentModel.CancelEventHandler(MainWindow_Closing);
            this.Show();

            game.Run();
        }

        void MainWindow_Closing(object sender, System.ComponentModel.CancelEventArgs e)
        {
            if (game != null)
            {
                game.Exit();
            }
        }

        private void bt_ok_Click(object sender, RoutedEventArgs e)
        {
            game.deltaAngle += 0.01f;
        }

        private void bt_cancel_Click(object sender, RoutedEventArgs e)
        {
            game.deltaAngle = 0.0f;
        }
    }
}
