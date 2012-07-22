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
        public MainWindow()
        {
            InitializeComponent();


            // test code
            Animation3D animation;
            try
            {
                animation = XMLLoader.Instance.load("../../test.xml");
            }
            catch (System.Exception e)
            {
                Console.WriteLine(e.ToString());
                return;
            }

            animation.print("../../output.xml", 0);
        }
    }
}
