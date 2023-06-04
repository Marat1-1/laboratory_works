using System;
using System.Diagnostics;
using System.IO;
using System.Windows.Forms;
using System.Threading;
using System.Runtime.InteropServices;
using static System.Windows.Forms.VisualStyles.VisualStyleElement;

namespace Csharp
{

    public partial class Form1 : Form
    {

        [DllImport("TranspFuncs.dll", CharSet = CharSet.Ansi)]
        private static extern void SendConnectionRequest(string nameClient);

        [DllImport("TranspFuncs.dll", CharSet = CharSet.Ansi)]
        private static extern void SendDisconnectRequest(string nameClient);

        [DllImport("TranspFuncs.dll", CharSet = CharSet.Ansi)]
        private static extern void StartThread(string nameClient);

        [DllImport("TranspFuncs.dll", CharSet = CharSet.Ansi)]
        private static extern void StopThread(string nameClient, int numberThread);

        [DllImport("TranspFuncs.dll", CharSet = CharSet.Ansi)]
        private static extern void SendSMS(string nameClient, int numberThread, string message);

        [DllImport("TranspFuncs.dll", CharSet = CharSet.Ansi)]
        private static extern void SendMessageServer(string nameClient, string message);

        [DllImport("TranspFuncs.dll", CharSet = CharSet.Ansi)]
        private static extern void CloseDialogue(string nameClient);

        public Form1()
        {
            InitializeComponent();
        }

        private int countCreateThreads = 0;

        private bool connection = false;

        private void Disconnected()
        {
            StopAllThreads();

            SendDisconnectRequest(this.Text);

            comboBox1.Items.Remove("Все потоки");
            comboBox1.Items.Remove("Сервер");
            countCreateThreads = 0;
            connection = false;
        }

        private void StopAllThreads()
        {
            while (comboBox1.Items.Count != 2)
            {
                StopThread(this.Text, int.Parse(comboBox1.Items[comboBox1.Items.Count - 1].ToString().Substring(6)));
                comboBox1.Items.RemoveAt(comboBox1.Items.Count - 1);
            }
        }

        private void SendMessageServer()
        {
            SendMessageServer(this.Text, textBox1.Text);
        }

        private bool getNumberThread(ref int numberThread)
        {
            if ((comboBox1.Text != string.Empty) && (int.TryParse(comboBox1.Text.Substring(6), out numberThread)))
            {
                return true;
            }
            else
            {
                return false;
            }
        }

        private void buttonStart_Click(object sender, EventArgs e)
        {
            if (connection)
            {            
                for (int i = 0; i < numericUpDown1.Value; ++i)
                {
                    StartThread(this.Text);

                    ++countCreateThreads;
                    comboBox1.Items.Add("Поток " + countCreateThreads.ToString());
                }               
            }
        }

        private void buttonStop_Click(object sender, EventArgs e)
        {
            if (connection)
            {
                if (comboBox1.Text == "Все потоки")
                {
                    StopAllThreads();
                }
                if (countCreateThreads != 0)
                {
                    int numberThread = -1;
                    if (getNumberThread(ref numberThread))
                    {
                        StopThread(this.Text, numberThread);

                        comboBox1.Items.Remove("Поток " + numberThread);
                    }
                }
            }            
        }

        private void buttonSend_Click(object sender, EventArgs e)
        {
            if (connection)
            {
                int numberThread = -1;
                switch (comboBox1.Text)
                {
                    case "Сервер":

                        SendMessageServer();

                        break;

                    case "Все потоки":
                        {
                            int countSendSMS = 0; 
                            SendMessageServer();
                            while (comboBox1.Items.Count - countSendSMS != 2)
                            {
                                SendSMS(this.Text, int.Parse(comboBox1.Items[comboBox1.Items.Count - 1].ToString().Substring(6)), textBox1.Text);
                                ++countSendSMS;
                            }
                            break;
                        }
                    default:
                        if (getNumberThread(ref numberThread))
                        {
                            SendSMS(this.Text, numberThread, textBox1.Text);
                        }
                        break;
                }
            }
        }

        private void Connect_Click(object sender, EventArgs e)
        {
            if (!connection)
            {
                countCreateThreads = 0;
                comboBox1.Items.Clear();

                SendConnectionRequest(this.Text);

                comboBox1.Items.Add("Сервер");
                comboBox1.Items.Add("Все потоки");
                connection = true;                
            }
        }

        private void Disconnect_Click(object sender, EventArgs e)
        {
            if (connection)
            {
                Disconnected();
            }
        }

        private void Form1_FormClosing(object sender, FormClosingEventArgs e)
        {
            if (connection)
            {
                Disconnected();
            }
            CloseDialogue(this.Text);
        }

    } 
       
}





