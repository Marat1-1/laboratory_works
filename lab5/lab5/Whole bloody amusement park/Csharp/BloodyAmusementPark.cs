using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Diagnostics;
using System.Runtime.InteropServices;
using System.Threading;
using System.IO;

namespace Csharp
{
    public partial class BloodyAmusementPark : Form
    {
        [DllImport("user32.dll")]
        private static extern IntPtr FindWindow(string windowClass, string windowName);

        [DllImport("user32.dll")]
        private static extern bool SetWindowText(IntPtr hWnd, string text);

        [DllImport("TranspFuncs.dll", CharSet = CharSet.Ansi)]
        private static extern void CloseServer();

        public BloodyAmusementPark()
        {
            InitializeComponent();
        }

        private bool conditionServer = false;

        private Process server = null;

        private Dictionary<string, Process> Clients = new Dictionary<string, Process>();

        private void buttonOnServer_Click(object sender, EventArgs e)
        {
            if (server == null || server.HasExited)
            {
                server = Process.Start("Server.exe");
                conditionServer = true;
            }
        }

        private void buttonOffServer_Click(object sender, EventArgs e)
        {            
            if (conditionServer)
            {
                CloseServer();
            }
        }

        private void buttonCreateClient_Click(object sender, EventArgs e)
        {
            if (conditionServer)
            {
                if (!Clients.ContainsKey(textBox1.Text))
                {
                    Clients.Add(textBox1.Text, Process.Start("Debug\\net6.0-windows\\Client.exe"));
                    Thread.Sleep(2000);
                    SetWindowText(Clients[textBox1.Text].MainWindowHandle, textBox1.Text);
                    Random rnd = new Random();
                    string[] setLegendphrases = { "1000-7",
                                          "Артём",
                                          "Дмитрий Андреевич",
                                          "Егор Саныч",
                                          "Ир Михална",
                                          "Жанна Дарковна",
                                          "Евгения Михайловна",
                                          "Легенда",
                                          "Человек",
                                          "Машина",
                                          "Академик",
                                          "Джим Рон",
                                          "Джон Генри Эдем",
                                          "Никита Капустин",
                                          "Mister X"
                                          };
                    this.textBox1.Text = setLegendphrases[rnd.Next(setLegendphrases.Length)];
                }
            }
        }

        private void buttonDeleteClient_Click(object sender, EventArgs e)
        {
            if (conditionServer)
            {
                Clients[textBox1.Text].CloseMainWindow();
                Clients.Remove(textBox1.Text);
            }
        }
    }
}
