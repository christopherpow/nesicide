from antlr4 import *
from rcLexer import rcLexer
from rcVisitor import rcVisitor
from rcParser import rcParser
import sys
import io
import codecs

class rcBitmap(object):
	id = None
	file = None

	def __init__(self,id,file):
		self.id = id
		self.file = '":/resources/'+(file.split("\\")[-1])

class rcBitmapList(object):
	bitmaps = []
	BitmapsHeader = \
"""
void qtMfcInitBitmapResources()
{
	qtMfcBitmapResources.clear();
"""
	BitmapItemFormat = \
"""   // {0} BITMAP {1}
	qtMfcBitmapResources.insert({0},new CBitmap({1}));
"""
	BitmapsFooter = \
"""}
"""

	def add(self,bitmap):
		self.bitmaps.append(bitmap)

	def output(self):
		print(self.BitmapsHeader)
		for bitmap in self.bitmaps:
			print(self.BitmapItemFormat.format(bitmap.id,bitmap.file))
		print(self.BitmapsFooter)

class rcIcon(object):
	id = None
	file = None

	def __init__(self,id,file):
		self.id = id
		self.file = '":/resources/'+(file.split("\\")[-1])

class rcIconList(object):
	icons = []
	IconsHeader = \
"""
void qtInitIconResources()
{
	qtIconNames.clear();
	qtIconResources.clear();

	// Icon with lowest ID value placed first to ensure application icon
	// remains consistent on all systems.
"""
	IconItemFormat = \
"""   // {0} ICON {1}
	qtIconNames.insert({0},{1});
"""
	IconsFooter = \
"""}
"""

	def add(self,icon):
		self.icons.append(icon)

	def output(self):
		print(self.IconsHeader)
		for icon in self.icons:
			print(self.IconItemFormat.format(icon.id,icon.file))
		print(self.IconsFooter)

class rcStringTable(object):
	StringTableItemFormat = \
"""   qtMfcStringResources.insert({0},"{1}");"""
	table = None

	def __init__(self,table):
		self.table = table

	def output(self):
		for i in range(len(self.table)):
			print(self.StringTableItemFormat.format(\
				self.table[i].ID().getText(),\
				self.table[i].String().getText()[1:-1].replace('""','\\\"')))

class rcStringTableList(object):
	stringTables = []
	StringTablesHeader = \
"""
void qtMfcInitStringResources()
{
	qtMfcStringResources.clear();
"""
	StringTableHeader = \
"""
	// STRINGTABLE
	// BEGIN"""
	StringTableFooter = \
"""   // END
"""
	StringTablesFooter = \
"""
	// AFX resources
	qtMfcStringResources.insert(AFX_IDS_ALLFILTER,"All files|");
	qtMfcStringResources.insert(AFX_IDS_OPENFILE,"Open");
	qtMfcStringResources.insert(AFX_IDS_SAVEFILE,"Save As");
	qtMfcStringResources.insert(AFX_IDS_SAVEFILECOPY,"Save As");
	qtMfcStringResources.insert(AFX_IDS_UNTITLED,"Untitled");
	qtMfcStringResources.insert(AFX_IDP_ASK_TO_SAVE,"Save changes to %s?");
	qtMfcStringResources.insert(AFX_IDP_FAILED_TO_CREATE_DOC,"Failed to create empty document.");
}
"""

	def add(self,stringTable):
		self.stringTables.append(stringTable)

	def output(self):
		print(self.StringTablesHeader)
		for stringTable in self.stringTables:
			print(self.StringTableHeader)
			stringTable.output()
			print(self.StringTableFooter)
		print(self.StringTablesFooter)

class rcToolbar(object):
	ToolbarsImplHeaderFormat = \
"""
void qtMfcInitToolBarResource_{0}(UINT dlgID,CToolBar* parent)
{{
	QImage toolBarImage = qtMfcBitmapResource({0})->toQPixmap()->toImage();
	QToolBar* toolBar = dynamic_cast<QToolBar*>(parent->toQWidget());
	QPixmap toolBarActionPixmap;
	QAction* toolBarAction;
	QRgb pixel00;
	int x,y;

	toolBarImage = toolBarImage.convertToFormat(QImage::Format_ARGB32);
	pixel00 = toolBarImage.pixel(0,0);
	for ( y = 0; y < toolBarImage.height(); y++ )
	{{
		for ( x = 0; x < toolBarImage.width(); x++ )
		{{
			if ( toolBarImage.pixel(x,y) == pixel00 )
			{{
				toolBarImage.setPixel(x,y,qRgba(0,0,0,0));
			}}
		}}
	}}
	
	// {0} TOOLBAR {1}, {2}
		toolBar->setIconSize(QSize({1},{2}));
	// BEGIN"""
	ToolbarsImplItemFormat = [
"""   // BUTTON {1}
	toolBarAction = new QAction(parent);
	toolBarActionPixmap = QPixmap::fromImage(toolBarImage.copy({0}*{2},0,{2},{3}));
	toolBarAction->setIcon(QIcon(toolBarActionPixmap));
	toolBarAction->setData({1});
	QObject::connect(toolBarAction,SIGNAL(triggered()),parent,SLOT(toolBarAction_triggered()));
	toolBar->addAction(toolBarAction);
""",
"""   // SEPARATOR
	toolBar->addSeparator();
"""
	]
	ToolbarsImplFooter = \
"""   // END

	for ( x = 0; x < toolBar->actions().count(); x++ )
	{
		CString toolTipCString = qtMfcStringResource(toolBar->actions().at(x)->data().toInt());
		QString toolTip = toolTipCString;
		if ( toolTip.indexOf('\\n') >= 0 )
		{
			toolBar->actions().value(x)->setToolTip(toolTip.right(toolTip.length()-toolTip.indexOf('\\n')-1));
		}
	}
}
"""
	ToolbarsDeclItemFormat = \
"""   case {0}:
		qtMfcInitToolBarResource_{0}(dlgID,parent);
		break;
"""
	BUTTON = 0
	SEPARATOR = 1
	X = 0
	Y = 1
	numbers = None
	table = None
	id = ""

	def __init__(self,id,numbers,table):
		self.id = id
		self.numbers = numbers
		self.table = table

	def output_decl(self):
		print(self.ToolbarsDeclItemFormat.format(self.id,self.numbers[self.X],self.numbers[self.Y]))

	def output_impl(self):
		print(self.ToolbarsImplHeaderFormat.format(self.id,self.numbers[self.X],self.numbers[self.Y]))
		idx = 0
		for item in self.table:
			if item.BUTTON():
				print(self.ToolbarsImplItemFormat[self.BUTTON].format(idx,item.ID().getText(),self.numbers[self.X],self.numbers[self.Y]))
				idx += 1
			else:
				print(self.ToolbarsImplItemFormat[self.SEPARATOR])
		print(self.ToolbarsImplFooter)

class rcToolbarList(object):
	ToolbarsDeclHeader = \
"""
void qtMfcInitToolBarResource(UINT dlgID,CToolBar* parent)
{
	switch ( dlgID )
	{"""
	ToolbarsDeclFooter = \
"""   }
}
"""
	toolbars = []

	def add(self,toolbar):
		self.toolbars.append(toolbar)

	def output(self):
		for toolbar in self.toolbars:
			toolbar.output_impl()

		print(self.ToolbarsDeclHeader)
		for toolbar in self.toolbars:
			toolbar.output_decl()
		print(self.ToolbarsDeclFooter)

class rcAcceleratorTable(object):
	AcceleratorTablesImplHeaderFormat = \
"""
// {0} ACCELERATORS
ACCEL ACCEL_{0}[] = 
{{
	// BEGIN"""
	AcceleratorTablesImplItemFormat = \
"""   // {1}, {0}, {2}
	{{ {2}, {1}, {0} }},
"""
	AcceleratorTablesImplFooter = \
"""   // END
	{ 0, 0, 0 },
};
"""
	AcceleratorTablesDeclItemFormat = \
"""      case {0}:
			return ACCEL_{0};
			break;
"""
	table = None
	id = ""

	def __init__(self,id,table):
		self.id = id
		self.table = table

	def output_decl(self):
		print(self.AcceleratorTablesDeclItemFormat.format(self.id))

	def output_impl(self):
		print(self.AcceleratorTablesImplHeaderFormat.format(self.id))
		for item in self.table:
			optionString = "F"+item.ACCELERATOR_TYPE().getText()+"|"
			if item.ACCELERATOR_OPTIONS():
				for idx in range(len(item.ACCELERATOR_OPTIONS())):
					optionString += "F"+item.ACCELERATOR_OPTIONS(idx).getText()
					if idx < len(item.ACCELERATOR_OPTIONS())-1:
						optionString += "|"
			if item.CONTROL():
				if optionString:
					optionString += "|"
				optionString += "F"+item.CONTROL(0).getText()
			eventString = ""
			if item.accelerator_event().String():
				eventString = "'"+item.accelerator_event().String().getText()[1]+"'"
			elif item.accelerator_event().ID():
				eventString = item.accelerator_event().ID().getText()
			elif item.accelerator_event().Number():
				eventString = item.accelerator_event().Number().getText()
			print(self.AcceleratorTablesImplItemFormat.format(item.ID().getText(),eventString,optionString))
		print(self.AcceleratorTablesImplFooter)

class rcAcceleratorTableList(object):
	AcceleratorTablesDeclHeader = \
"""
ACCEL* qtMfcAcceleratorResource(UINT id)
{
	switch ( id )
	{"""
	AcceleratorTablesDeclFooter = \
"""   }
	return NULL;
}
"""
	accleratorTables = []

	def add(self,acceleratorTable):
		self.accleratorTables.append(acceleratorTable)

	def output(self):
		for acceleratorTable in self.accleratorTables:
			acceleratorTable.output_impl()

		print(self.AcceleratorTablesDeclHeader)
		for acceleratorTable in self.accleratorTables:
			acceleratorTable.output_decl()
		print(self.AcceleratorTablesDeclFooter)


class rcMenu(object):
	MenusImplHeaderFormat = \
"""
void qtMfcInitMenuResource_{0}(CMenu* parent)
{{
	QAction* menuAction;

	// {0} MENU
	// BEGIN
	QList<CMenu*> subMenuTree;
	CMenu* subMenu;
"""
	MenusImplFooter = \
"""   // END
}
"""
	MenusDeclItemFormat = \
"""      case {0}:
			qtMfcInitMenuResource_{0}(parent);
			break;
"""
	PopupsImplHeaderFormat = \
"""
	// POPUP {0}
	subMenu = new CMenu;
	subMenuTree.append(subMenu);
	QObject::connect(subMenu->toQMenu(),SIGNAL(aboutToShow()),subMenu,SLOT(menu_aboutToShow()));
	if ( subMenuTree.count() == 1 )
		parent->AppendMenu(MF_POPUP|MF_STRING,(UINT_PTR)subMenu->m_hMenu,{0});
	else
		subMenuTree.at(subMenuTree.count()-2)->AppendMenu(MF_POPUP|MF_STRING,(UINT_PTR)subMenu->m_hMenu,{0});
		
	// BEGIN"""
	PopupsImplFooter = \
"""   // END
	subMenu = subMenuTree.takeLast();
"""
	MenuItemFormat = [
"""   // MENUITEM {1}, {0}
	subMenu->AppendMenu({2},{0},{1});
""",
"""   // MENUITEM SEPARATOR
	subMenu->AppendMenu({0});
"""
]
	id = ""
	MENUITEM = 0
	SEPARATOR = 1
	table = None

	def __init__(self, id, table):
		self.id = id
		self.table = table

	def output_decl(self):
		print(self.MenusDeclItemFormat.format(self.id))

	def output_popup(self, popup):
		print(self.PopupsImplHeaderFormat.format(popup.String().getText()))
		for child in popup.children:
			if type(child) is rcParser.Menu_itemContext:
				optionString = ""
				if child.MENU_OPTIONS():
					for idx in range(len(child.MENU_OPTIONS())):
						optionString += "MF_" + child.MENU_OPTIONS(idx).getText()
						if idx < len(child.MENU_OPTIONS()) - 1:
							optionString += "|"
				if child.SEPARATOR():
					if optionString != "":
						optionString += "|"
					optionString += "MF_SEPARATOR"
					print(self.MenuItemFormat[self.SEPARATOR].format(optionString))
				elif child.MENUITEM():
					if optionString != "":
						optionString += "|"
					optionString += "MF_STRING"
					print(self.MenuItemFormat[self.MENUITEM].format(child.ID().getText(), child.String().getText(),
																					optionString))
			elif type(child) is rcParser.Popup_itemContext:
				self.output_popup(child)
		print(self.PopupsImplFooter)

	def output_impl(self):
		print(self.MenusImplHeaderFormat.format(self.id, "?????"))
		for item in self.table:
			self.output_popup(item)
		print(self.MenusImplFooter)


class rcMenuList(object):
	MenusDeclHeader = \
"""
void qtMfcInitMenuResource(UINT menuID,CMenu* parent)
{
	switch ( menuID )
	{"""
	MenusDeclFooter = \
"""   }

	// Fixup shortcuts
	int menu = 0;
	CMenu* subMenu = parent->GetSubMenu(menu);
	while ( subMenu )
	{
		foreach ( QAction* action, subMenu->toQMenu()->actions() )
		{
			if ( action->text().contains("\\t") )
			{
				action->setShortcut(QKeySequence(action->text().split("\\t").at(1)));
			}
		}
		menu++;
		subMenu = parent->GetSubMenu(menu);
	}
}
"""
	menus = []

	def add(self, menu):
		self.menus.append(menu)

	def output(self):
		for menu in self.menus:
			menu.output_impl()

		print(self.MenusDeclHeader)
		for menu in self.menus:
			menu.output_decl()
		print(self.MenusDeclFooter)


class rcDialogEx(object):
	DialogExImplHeaderStartFormat = \
"""
void qtMfcInitDialogResource_{0}(CDialog* parent)
{{
	QHash<int,CWnd*>* mfcToQtWidget = parent->mfcToQtWidgetMap();
	
	// {0} DIALOGEX {1}, {2}, {3}, {4}
	CRect rect(CPoint({1},{2}),CSize({3},{4}));
	parent->MapDialogRect(&rect);
	parent->setFixedSize(rect.Width(),rect.Height());
"""
	CaptionFormat = \
"""   // CAPTION {0}
	parent->SetWindowText({0});
"""
	StyleFormat = \
"""   // STYLE {0}
	parent->toQWidget()->setWindowFlags(parent->toQWidget()->windowFlags(){1});
"""
	FontFormat = \
"""   // FONT {0}, {1}, {2}, {3}, {4}
"""
	DialogExImplHeaderEnd = \
"""   // BEGIN"""
	DialogExDeclItemFormat = \
"""      case {0}:
			qtMfcInitDialogResource_{0}(parent);
			break;
"""
	DialogExImplFooter = \
"""   // END
}
"""
	id = ""
	items = None
	x = y = dx = dy = 0
	caption = ""
	style = ""
	font = ""
	numbers = None

	def __init__(self, id, items, options, numbers):
		self.id = id
		self.items = items
		self.x = numbers[0]
		self.y = numbers[1]
		self.dx = numbers[2]
		self.dy = numbers[3]
		for option in options:
			if option.CAPTION():
				self.caption = self.CaptionFormat.format(option.String().getText())
			if option.FONT():
				pass
			if option.STYLE():
				stylesOrig = self.get_styles(option.stylelist(), "")
				if stylesOrig:
					styles = stylesOrig
					styles = styles.replace("WS_CAPTION","Qt::WindowTitleHint")
					styles = styles.replace("WS_POPUP","Qt::Popup")
					styles = styles.replace("WS_SYSMENU","Qt::WindowSystemMenuHint")
					styles = styles.replace("WS_MAXIMIZEBOX","Qt::WindowMaximizeButtonHint")
					styles = styles.replace("WS_MINIMIZEBOX","Qt::WindowMinimizeButtonHint")
				if styles:
					styles = styles.split("|")
					styles = [ x for x in styles if x.startswith("Qt::") ]
				if styles:
					styles = "|".join(styles)
					if styles != "":
						styles = "|"+styles
						self.style = self.StyleFormat.format(stylesOrig,styles)

	def output_decl(self):
		print(self.DialogExDeclItemFormat.format(self.id))

	def get_styles(self,stylelist,styles):
		if stylelist:
			if stylelist.stylelist():
				styles = self.get_styles(stylelist.stylelist(),styles)
			if stylelist.NOT() == None:
				if styles != "":
					styles += "|"
				styles += stylelist.any_style().getText()
			elif "|"+stylelist.any_style().getText() in styles:
				styles = styles.replace("|"+stylelist.any_style().getText(), '')
			elif stylelist.any_style().getText()+"|" in styles:
				styles = styles.replace(stylelist.any_style().getText()+"|", '')
			elif stylelist.any_style().getText() in styles:
				styles = styles.replace(stylelist.any_style().getText(), '')

		return styles

	def output_generic_control_statement(self,idx,item):
		ItemFormat = \
"""   // CONTROL {3}, {1}, {4}, {2}, {6}, {7}, {8}, {9}
	{5}* mfc{0} = new {5}(parent);
	CRect r{0}(CPoint({6}, {7}), CSize({8}, {9}));
	parent->MapDialogRect(&r{0});
	mfc{0}->Create(_T({3}), {2}, r{0}, parent, {1});
	mfcToQtWidget->insert({1}, mfc{0});
"""
		ItemFormatNoTitle = \
"""   // CONTROL {3}, {1}, {4}, {2}, {6}, {7}, {8}, {9}
	{5}* mfc{0} = new {5}(parent);
	CRect r{0}(CPoint({6}, {7}), CSize({8}, {9}));
	parent->MapDialogRect(&r{0});
	mfc{0}->Create({2}, r{0}, parent, {1});
	mfcToQtWidget->insert({1}, mfc{0});
"""
		styles = self.get_styles(item.stylelist(),"WS_VISIBLE|WS_BORDER")
		ItemFormatInUse = ItemFormatNoTitle
		mfcClass = ""
		if item.String(1).getText().lower() == '"button"':
			mfcClass = "CButton"
			ItemFormatInUse = ItemFormat
		elif item.String(1).getText().lower() == '"static"':
			mfcClass = "CStatic"
			ItemFormatInUse = ItemFormat
		elif item.String(1).getText().lower() == '"syslistview32"':
			mfcClass = "CListCtrl"
		elif item.String(1).getText().lower() == '"msctls_trackbar32"':
			mfcClass = "CSliderCtrl"
		elif item.String(1).getText().lower() == '"msctls_updown32"':
			mfcClass = "CSpinButtonCtrl"
		elif item.String(1).getText().lower() == '"msctls_progress32"':
			mfcClass = "CProgressCtrl"
		elif item.String(1).getText().lower() == '"systabcontrol32"':
			mfcClass = "CTabCtrl"
		elif item.String(1).getText().lower() == '"systreeview32"':
			mfcClass = "CTreeCtrl"

		print(ItemFormatInUse.format(idx, \
										item.ID().getText(), \
										styles,
										item.String(0).getText(), \
										item.String(1).getText(), \
										mfcClass, \
										item.Number(0).getText(), \
										item.Number(1).getText(), \
										item.Number(2).getText(), \
										item.Number(3).getText() \
										))
		return item.ID().getText()

	def output_static_control_statement(self,idx,item):
		ItemFormat = \
"""   // {3} {4},{1},{5},{6},{7},{8}
	CStatic* mfc{0} = new CStatic(parent);
	CRect r{0}(CPoint({5},{6}),CSize({7},{8}));
	parent->MapDialogRect(&r{0});
	mfc{0}->Create(_T({4}),{2},r{0},parent,{1});
	// IDC_STATIC do not get added to MFC-to-Qt map.
"""
		styles = self.get_styles(item.stylelist(),"WS_VISIBLE|WS_BORDER")

		print(ItemFormat.format(idx, \
										item.ID().getText(), \
										styles, \
										item.static_class_identifier().getText(), \
										item.String().getText(), \
										item.Number(0).getText(), \
										item.Number(1).getText(), \
										item.Number(2).getText(), \
										item.Number(3).getText(), \
										))
		return item.ID().getText()

	def output_groupbox_control_statement(self,idx,item):
		ItemFormat = \
"""   // GROUPBOX {3},{1},{4},{5},{6},{7}
	CButton* mfc{0} = new CButton(parent);
	CRect r{0}(CPoint({4},{5}),CSize({6},{7}));
	parent->MapDialogRect(&r{0});
	mfc{0}->Create(_T({3}),{2},r{0},parent,{1});
	// IDC_STATIC do not get added to MFC-to-Qt map.
"""
		styles = self.get_styles(item.stylelist(),"BS_GROUPBOX|WS_VISIBLE|WS_BORDER")

		print(ItemFormat.format(idx, \
										item.ID().getText(), \
										styles, \
										item.String().getText(), \
										item.Number(0).getText(), \
										item.Number(1).getText(), \
										item.Number(2).getText(), \
										item.Number(3).getText(), \
										))
		return item.ID().getText()

	def output_combobox_control_statement(self,idx,item):
		ItemFormat = \
"""   // COMBOBOX {1},{3},{4},{5},{6},{2}
	CComboBox* mfc{0} = new CComboBox(parent);
	CRect r{0}(CPoint({3},{4}),CSize({5},{6}));
	parent->MapDialogRect(&r{0});
	mfc{0}->Create({2},r{0},parent,{1});
	mfcToQtWidget->insert({1},mfc{0});
"""
		styles = self.get_styles(item.stylelist(),"WS_VISIBLE|WS_BORDER")

		print(ItemFormat.format(idx, \
										item.ID().getText(), \
										styles, \
										item.Number(0).getText(), \
										item.Number(1).getText(), \
										item.Number(2).getText(), \
										item.Number(3).getText(), \
										))
		return item.ID().getText()

	def output_listbox_control_statement(self,idx,item):
		ItemFormat = \
"""   // LISTBOX {1},{3},{4},{5},{6},{2}
	CListBox* mfc{0} = new CListBox(parent);
	CRect r{0}(CPoint({3},{4}),CSize({5},{6}));
	parent->MapDialogRect(&r{0});
	mfc{0}->Create({2},r{0},parent,{1});
	mfcToQtWidget->insert({1},mfc{0});
"""
		styles = self.get_styles(item.stylelist(),"WS_VISIBLE|WS_BORDER")

		print(ItemFormat.format(idx, \
										item.ID().getText(), \
										styles, \
										item.Number(0).getText(), \
										item.Number(1).getText(), \
										item.Number(2).getText(), \
										item.Number(3).getText(), \
										))
		return item.ID().getText()

	def output_defpushbutton_control_statement(self,idx,item):
		ItemFormat = \
"""   // DEFPUSHBUTTON {3},{2},{4},{5},{6},{7}
	CButton* mfc{0} = new CButton(parent);
	CRect r{0}(CPoint({4},{5}),CSize({6},{7}));
	parent->MapDialogRect(&r{0});
	mfc{0}->Create(_T({3}),{2},r{0},parent,{1});
	mfcToQtWidget->insert({1},mfc{0});
"""
		styles = self.get_styles(item.stylelist(),"BS_DEFPUSHBUTTON|WS_VISIBLE|WS_BORDER")

		print(ItemFormat.format(idx, \
										item.ID().getText(), \
										styles, \
										item.String().getText(), \
										item.Number(0).getText(), \
										item.Number(1).getText(), \
										item.Number(2).getText(), \
										item.Number(3).getText(), \
										))
		return item.ID().getText()

	def output_edit_control_statement(self,idx,item):
		ItemFormat = \
"""   // {1} {2},{4},{5},{6},{7},{3}
	CEdit* mfc{0} = new CEdit(parent);
	CRect r{0}(CPoint({4},{5}),CSize({6},{7}));
	parent->MapDialogRect(&r{0});
	mfc{0}->Create({3},r{0},parent,{2});
	mfcToQtWidget->insert({2},mfc{0});
"""
		styles = self.get_styles(item.stylelist(),"WS_VISIBLE|WS_BORDER")

		print(ItemFormat.format(idx, \
										item.edit_class_identifier().getText(),
										item.ID().getText(), \
										styles, \
										item.Number(0).getText(), \
										item.Number(1).getText(), \
										item.Number(2).getText(), \
										item.Number(3).getText(), \
										))
		return item.ID().getText()

	def output_button_control_statement(self,idx,item):
		ItemFormat = \
"""   // {1} {4},{2},{5},{6},{7},{8},{3}
	CButton* mfc{0} = new CButton(parent);
	CRect r{0}(CPoint({5},{6}),CSize({7},{8}));
	parent->MapDialogRect(&r{0});
	mfc{0}->Create(_T({4}),{3},r{0},parent,{2});
	mfcToQtWidget->insert({2},mfc{0});
"""
		styles = self.get_styles(item.stylelist(),"WS_VISIBLE|WS_BORDER")

		print(ItemFormat.format(idx, \
										item.button_class_identifier().getText(),
										item.ID().getText(), \
										styles, \
										item.String().getText(), \
										item.Number(0).getText(), \
										item.Number(1).getText(), \
										item.Number(2).getText(), \
										item.Number(3).getText(), \
										))
		return item.ID().getText()

	def output_dialog_item(self,idx,item,groupboxPriority):
		if groupboxPriority == True:
			if item.groupbox_control_statement():
				_ = self.output_groupbox_control_statement(idx, item.groupbox_control_statement())
				return 1
			return 0
		subId = None
		if item.generic_control_statement():
			subId = self.output_generic_control_statement(idx,item.generic_control_statement())
		elif item.static_control_statement():
			subId = self.output_static_control_statement(idx,item.static_control_statement())
		elif item.combobox_control_statement():
			subId = self.output_combobox_control_statement(idx, item.combobox_control_statement())
		elif item.listbox_control_statement():
			subId = self.output_listbox_control_statement(idx, item.listbox_control_statement())
		elif item.edit_control_statement():
			subId = self.output_edit_control_statement(idx, item.edit_control_statement())
		elif item.button_control_statement():
			subId = self.output_button_control_statement(idx, item.button_control_statement())
		elif item.defpushbutton_control_statement():
			subId = self.output_defpushbutton_control_statement(idx, item.defpushbutton_control_statement())
		if subId:
			dialogInitList.output(self.id, subId, idx)
		return 1

	def output_impl(self):
		print(self.DialogExImplHeaderStartFormat.format(self.id,
																 self.x,
																 self.y,
																 self.dx,
																 self.dy))
		print(self.caption)
		print(self.style)
		print(self.font)
		print(self.DialogExImplHeaderEnd)
		idx = 0
		for item in self.items:
			idx += self.output_dialog_item(idx,item,True)
		for item in self.items:
			idx += self.output_dialog_item(idx,item,False)
		print(self.DialogExImplFooter)


class rcDialogExList(object):
	DialogExsDeclHeader = \
"""
void qtMfcInitDialogResource(UINT dlgID,CDialog* parent)
{
	switch ( dlgID )
	{"""
	DialogExsDeclFooter = \
"""
	case 0:
		// CP: Allow blank dialogs.
		break;
	default:
		qFatal("dialog resource not implemented...");
	}
}
"""
	dialogs = []

	def add(self, dialog):
		self.dialogs.append(dialog)

	def output(self):
		for dialog in self.dialogs:
			dialog.output_impl()

		print(self.DialogExsDeclHeader)
		for dialog in self.dialogs:
			dialog.output_decl()
		print(self.DialogExsDeclFooter)


class rcDialogInitList(object):
	DialogInitItemFormat = """
	mfc{0}->AddString(_T("{1}"));
	"""
	initLists = dict()

	def addList(self, id):
		self.initLists[str(id)] = dict()

	def addItem(self, id, subId, item):
		if not str(subId) in self.initLists[str(id)]:
			self.initLists[str(id)][str(subId)] = list()
		self.initLists[str(id)][str(subId)].append(item)

	def output(self, id, subId, mfc):
		if str(id) in self.initLists:
			itemsForId = self.initLists[str(id)]
			if str(subId) in itemsForId:
				items = itemsForId[str(subId)]
				if items:
					for item in items:
						if len(item)%2:
							item += "0"
						print(self.DialogInitItemFormat.format(mfc,item))#.decode('hex')))

class myRcVisitor(rcVisitor):
	def visitToolbar_statement(self, ctx):
		toolbarList.add(rcToolbar(ctx.ID().getText(),ctx.Number(),ctx.toolbar_item()))

	def visitBitmap_statement(self, ctx):
		bitmapList.add(rcBitmap(ctx.ID().getText(),ctx.String().getText()))

	def visitIcon_statement(self, ctx):
		iconList.add(rcIcon(ctx.ID().getText(),ctx.String().getText()))

	def visitStringtable_statement(self, ctx):
		stringTableList.add(rcStringTable(ctx.stringtable_item()))

	def visitAccelerator_statement(self, ctx):
		acceleratorTableList.add(rcAcceleratorTable(ctx.ID().getText(),ctx.accelerator_item()))

	def visitMenu_statement(self, ctx):
		menuList.add(rcMenu(ctx.ID().getText(),ctx.popup_item()))

	def visitDialogex_statement(self, ctx):
		dialogExList.add(rcDialogEx(ctx.ID(0).getText(), # ID(0) is dialog ID, ID(1) is help ID
											 ctx.dialog_item(),
											 ctx.dialog_option(),
											 ctx.Number()))

	def swap_order(self,d, wsz=4, gsz=2):
		return "".join(["".join([m[i:i + gsz] for i in range(wsz - gsz, -gsz, -gsz)]) for m in
		                [d[i:i + wsz] for i in range(0, len(d), wsz)]])

	def visitDialoginit_statement(self, ctx):
		dialogInitList.addList(ctx.ID().getText())
		if ctx.dialoginit_item():
			for item in ctx.dialoginit_item():
				if item.dialoginit_data():
					itemToAdd = ""
					for data in item.dialoginit_data():
						itemToAdd += data.getText().strip(",").replace("0x","").strip('"\\')
						if len(itemToAdd)%2:
							itemToAdd += "0"
					itemToAdd = self.swap_order(itemToAdd)
					while itemToAdd[-2:] == "00":
						itemToAdd = itemToAdd[:-2]
					dialogInitList.addItem(ctx.ID().getText(),item.ID().getText(),itemToAdd)

bitmapList = rcBitmapList()
iconList = rcIconList()
stringTableList = rcStringTableList()
toolbarList = rcToolbarList()
acceleratorTableList = rcAcceleratorTableList()
menuList = rcMenuList()
dialogExList = rcDialogExList()
dialogInitList = rcDialogInitList()

FileHeader = \
"""#include "cqtmfc.h"
#include "cqtmfc_famitracker.h"
#include "resource.h"

#include "stdafx.h"

#include "FamiTrackerDoc.h"
"""
FileFooter = \
"""
void qtMfcInit(QMainWindow* parent)
{
	// Hook Qt to this MFC app...
	AfxGetApp()->qtMainWindow = parent;
	AfxGetApp()->moveToThread(QApplication::instance()->thread());

	qtMfcInitStringResources();
	qtMfcInitBitmapResources();
	qtInitIconResources();
}
"""

def main(argv):
	output = io.StringIO()

	input = FileStream(argv[1])
	lexer = rcLexer(input)
	stream = CommonTokenStream(lexer)
	parser = rcParser(stream)
	tree = parser.rc()
	visitor = myRcVisitor()
	traverseResult = visitor.visit(tree)

	print(FileHeader)
	menuList.output()
	stringTableList.output()
	acceleratorTableList.output()
	bitmapList.output()
	iconList.output()
	dialogExList.output()
	toolbarList.output()
	print(FileFooter)

if __name__ == '__main__':
	 main(sys.argv)
