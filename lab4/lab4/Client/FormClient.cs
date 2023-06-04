using System;
using System.Diagnostics;
using System.IO;
using System.Windows.Forms;
using System.Threading;
using System.Runtime.InteropServices;
using System.Text;
using static System.Windows.Forms.VisualStyles.VisualStyleElement;

enum Command {
    Connection = 0,
    DisconnectingConnection = 1,
    Start = 2,
    Stop = 3,
    Send = 4,
    CloseDialogue = 5,
    SendMessageServer = 6
}

namespace Csharp
{
    public partial class FormClient : Form
    {

        [DllImport("../../TranspFuncs.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr SendConnectionRequest(string nameClient, IntPtr updateInfo);

        [DllImport("../../TranspFuncs.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr Update(string nameClient, IntPtr updateInfo);

        [DllImport("../../TranspFuncs.dll", CharSet = CharSet.Ansi)]
        private static extern void SendDisconnectRequest(string nameClient);

        [DllImport("../../TranspFuncs.dll", CharSet = CharSet.Ansi)]
        private static extern void StartThread(string nameClient);

        [DllImport("../../TranspFuncs.dll", CharSet = CharSet.Ansi)]
        private static extern bool StopThread(string nameClient, int numberThread);

        [DllImport("../../TranspFuncs.dll", CharSet = CharSet.Ansi)]
        private static extern void StopAllThreads(string nameClient);

        [DllImport("../../TranspFuncs.dll", CharSet = CharSet.Ansi)]
        private static extern bool SendSMS(string nameClient, int numberThread, string message);

        [DllImport("../../TranspFuncs.dll", CharSet = CharSet.Ansi)]
        private static extern void SendSMSToServer(string nameClient, string message);

        [DllImport("../../TranspFuncs.dll", CharSet = CharSet.Ansi)]
        private static extern void SendSMSAll(string nameClient, string message);

        public FormClient()
        {
            InitializeComponent();
            this.Text = "Безымянный";
        }

        private bool connection = false;

        private void Disconnected()
        {
            SendDisconnectRequest(this.Text);

            comboBox1.Items.Clear();

            connection = false;
        }

        void UpdateTextBox()
        {
            //Болавство:
            //Склонность потворствовать чьим-то желаниям, прихотям, капризам;
            //относиться к кому-либо с особым вниманием, заботой, лаской; доставлять удовольствие,
            //радость кому-либо, делать что-либо приятное. 
            Random rnd = new Random();
            string[] setLegendphrases = { "Здравствуй прекрасная кафедра АСУ",
                                          "Здравствуй прекрасная АА-20-05",
                                          "Здравствуй прекрасная АС-20-04",
                                          "Я тебя не боюсь",
                                          "Не пиши сюда",
                                          "Не обижайте гулечек",
                                          "Это вам не это",
                                          "А восход всё также прекрасен",
                                          "Не тяни меня на дно",
                                          "Кто у вас тут ещё есть остальной?",
                                          "И вот остался он один в сознанье собственных глубин",
                                          "Теперь мой взор не возмутим, машина предстоит пред ним",
                                          "Мы все должны испытывать два вида боли: боль дисциплины и боль сожаления. Разница в том, что боль дисциплины весит граммы, в то время как боль сожалений весит тонны.\r\n—  Джим Рон",
                                          "Не заглатывай наживку удовольствия не убидившись, что на ней нет крючка.\r\n—  Джон Генри Эдем",
                                          "... и гений, что вечно бороздит необъятной мыслью океан"
                                          };
            this.textBox1.Text = setLegendphrases[rnd.Next(setLegendphrases.Length)];
        }

        void UpdateComboBox()
        {
            comboBox1.Items.Clear();

            string strBuf = new string(Enumerable.Repeat<char>('0', 3000).ToArray());
            int[] numberThreads = StringNumberThreadsToArray(Marshal.PtrToStringAnsi(Update(this.Text, Marshal.StringToHGlobalAnsi(strBuf))));

            comboBox1.Items.Add("Сервер");
            comboBox1.Items.Add("Все потоки");
            foreach (var number in numberThreads)
            {
                comboBox1.Items.Add("Поток " + number.ToString());
            }

        }
        void UpdateComboBox(int[] numberThreads)
        {
            comboBox1.Items.Clear();

            comboBox1.Items.Add("Сервер");
            comboBox1.Items.Add("Все потоки");
            foreach (var number in numberThreads)
            {
                comboBox1.Items.Add("Поток " + number.ToString());
            }

        }

        private int[] StringNumberThreadsToArray(string str)
        {
            int[] bufInt = { };
            if (str.Length != 0)
            {
                return str.Split('|').Select(int.Parse).ToArray();
            }
            else
            {
                return bufInt; 
            }
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
                //UpdateComboBox();
                for (int i = 0; i < numericUpDown1.Value; ++i)
                {
                    StartThread(this.Text);
                    comboBox1.Items.Add("Поток " + (comboBox1.Items.Count - 1).ToString());
                }               
            }
        }

        private void buttonStop_Click(object sender, EventArgs e)
        {
            if (connection)
            {
                if (comboBox1.Text == "Все потоки")
                {
                    StopAllThreads(this.Text);

                    comboBox1.Items.Clear();
                    comboBox1.Items.Add("Сервер");
                    comboBox1.Items.Add("Все потоки");
                }
                else 
                {
                    int numberThread = -1;
                    if (getNumberThread(ref numberThread))
                    {
                        if (StopThread(this.Text, numberThread))
                        {
                            comboBox1.Items.Remove("Поток " + numberThread);
                        }
                        else
                        {
                            UpdateComboBox();
                        }                       
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
                        { 
                            SendSMSToServer(this.Text, textBox1.Text);

                            UpdateTextBox();
                            break;
                        }
                    case "Все потоки":
                        {
                            SendSMSAll(this.Text, textBox1.Text);

                            UpdateTextBox();
                            break;
                        }
                    default:
                        if (getNumberThread(ref numberThread))
                        {
                            if (SendSMS(this.Text, numberThread, textBox1.Text))
                            {
                                UpdateTextBox();
                            }
                            else
                            {
                                UpdateComboBox();
                            }
                        }
                        break;
                }
            }
        }

        private void Connect_Click(object sender, EventArgs e)
        {
            if (!connection)
            {

                //StringBuilder charBuf = new StringBuilder(3000);
                string strBuf = new string(Enumerable.Repeat<char>('0', 3000).ToArray());
                int[] numberThreads = StringNumberThreadsToArray(Marshal.PtrToStringAnsi(SendConnectionRequest(this.Text, Marshal.StringToHGlobalAnsi(strBuf))));

                UpdateComboBox(numberThreads);
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
        }

        private void update_Click(object sender, EventArgs e)
        {
            if (connection)
            {
                UpdateComboBox();
            }
        }
    } 
       
}





