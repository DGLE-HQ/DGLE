/**
\author		Shestakov Mikhail aka MIKE
\date		29.10.2012 (c)Andrey Korotkov

This file is a part of DGLE2 project and is distributed
under the terms of the GNU Lesser General Public License.
See "DGLE2.h" for more details.
*/
using System;

namespace Gui
{
	public partial class CustomMessageDialog : Gui.CustomWindow
	{
		private static int ICON_SIZE = 48;

		public string Text {
			get { return lText.Text; }
			set { lText.Text = value; }
		}

		public Gtk.MessageType MessageType {
			get { return messageType; }
			set {
				messageType = value;
				SetIcon ();
			}
		}

		public Gtk.ButtonsType ButtonsType {
			get { return buttonsType; }
			set {
				buttonsType = value;
				SetButtons ();
			}
		}

		public event EventHandler Ok
		{
			add { this.btn1.Clicked += value; }
			remove { this.btn1.Clicked -= value; }
		}
		
		public event EventHandler Cancel
		{
			add { this.btn2.Clicked += value; }
			remove { this.btn2.Clicked -= value; }
		}

		public event EventHandler Close
		{
			add { this.btn2.Clicked += value; }
			remove { this.btn2.Clicked -= value; }
		}

		public event EventHandler Yes
		{
			add { this.btn1.Clicked += value; }
			remove { this.btn1.Clicked -= value; }
		}

		public event EventHandler No
		{
			add { this.btn2.Clicked += value; }
			remove { this.btn2.Clicked -= value; }
		}

		public Gtk.ButtonsType buttonsType;
		private Gtk.MessageType messageType;

		public CustomMessageDialog (Gtk.Window parent, string format, object[] args) : 
			this (parent, Gtk.MessageType.Other, Gtk.ButtonsType.Ok, "", String.Format(format, args))
		{
		}

		public CustomMessageDialog (Gtk.Window parent, string title, string format, object[] args) : 
			this (parent, Gtk.MessageType.Other, Gtk.ButtonsType.Ok, title, String.Format(format, args))
		{
		}

		public CustomMessageDialog (Gtk.Window parent, string text) : 
			this (parent, Gtk.MessageType.Other, Gtk.ButtonsType.Ok, "", text)
		{
		}

		public CustomMessageDialog (Gtk.Window parent, string title, string text) : 
			this (parent, Gtk.MessageType.Other, Gtk.ButtonsType.Ok, title, text)
		{
		}

		public CustomMessageDialog (
			Gtk.Window parent, 
			Gtk.MessageType messageType, 
			Gtk.ButtonsType buttonsType, 
			string text) :
			this (parent, messageType, buttonsType, "", text)
		{
		}

		public CustomMessageDialog (
			Gtk.Window parent, 
			Gtk.MessageType messageType, 
			Gtk.ButtonsType buttonsType, 
			string title, string text) : base(Gtk.WindowType.Toplevel)
		{
			this.Build ();

			base.TransientFor = parent;
			base.SetPosition (Gtk.WindowPosition.CenterOnParent);
			base.Decorated = parent.Decorated;
			base.KeepAbove = true;
			
			base.Title = title;
			Text = text;
			MessageType = messageType;
			ButtonsType = buttonsType;

			this.btn1.Clicked += HandleMessageDialogClicked;
			this.btn2.Clicked += HandleMessageDialogClicked;
			this.DeleteEvent += delegate(object o, Gtk.DeleteEventArgs args) {
				this.btn2.Click ();
			};
		}

		void HandleMessageDialogClicked (object sender, EventArgs e)
		{
			this.Hide ();
		}

		private void SetIcon ()
		{
			if ((int)messageType < 0 || (int)messageType >= (int)Gtk.MessageType.Other) {
				this.imageType.Pixbuf = null;
			} else {
				string iconName = String.Format ("gtk-dialog-{0}", messageType.ToString ()).ToLower ();
				if (null != Gtk.IconTheme.Default.LookupIcon (iconName, ICON_SIZE, Gtk.IconLookupFlags.UseBuiltin))
					this.imageType.Pixbuf =
						Gtk.IconTheme.Default.LoadIcon (iconName, ICON_SIZE, Gtk.IconLookupFlags.UseBuiltin);
				else
					this.imageType.Pixbuf = null;
			}
		}

		private void SetButtons ()
		{
			switch (buttonsType) {
			case Gtk.ButtonsType.Ok:
				btn1.Label = "Ok";
				btn2.Visible = false;
				btn2.Sensitive = false;
				break;
			case Gtk.ButtonsType.Close:
				btn1.Visible = false;
				btn1.Sensitive = false;
				btn2.Label = "Close";
				break;
			case Gtk.ButtonsType.Cancel:
				btn1.Visible = false;
				btn1.Sensitive = false;
				btn2.Label = "Cancel";
				break;
			case Gtk.ButtonsType.OkCancel:
				btn1.Label = "Ok";
				btn2.Label = "Cancel";
				break;
			case Gtk.ButtonsType.YesNo:
				btn1.Label = "Yes";
				btn2.Label = "No";
				break;
			default:
				this.hbtnboxAction.Visible = false;
				this.hbtnboxAction.Sensitive = false;
				break;
			}
		}
	}
}

