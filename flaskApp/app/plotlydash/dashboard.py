from app import mongo
import dash
import dash_table
import dash_html_components as html
import dash_core_components as dcc
from dash.dependencies import Input, Output, State
import pandas as pd
import pdb

def get_graph():
    df = pd.DataFrame(list(mongo.db.temp.aggregate([{'$sort': {'time': 1}}])))

    by_node = df.groupby('node')
    data = []
    for node, values in by_node:
        data.append({
            'x'     : values['time'],
            'y'     : values['temp'],
            'name'  : 'Node ' + str(node),
        })
    return data

def create_dashboard(server):
    dash_app = dash.Dash(server=server,
                         routes_pathname_prefix='/dashapp/',
                         external_stylesheets=['/static/css/styles.css']
                         )

    data = get_graph()

    # Create layout
    dash_app.layout = html.Div(
        children=[html.Div( id='left', children = [
        dcc.Graph(
            id='temp-graph',
            figure={
                'data': data,
                'layout': {
                    'title': 'Temperature Graph',
                    'height': 600,
                    'padding': 150,
                    'xaxis': {'title': {'text': 'Time of Reading (UTC)'}},
                    'yaxis': {'title': {'text': 'Temperature (°C)'}},
                }
            }
        )]), html.Div( id='right', children = [
        html.Button('Refresh Graph', id='refresh-graph', n_clicks=0),
        dcc.Input(
            id='node-input',
            placeholder='Enter node number to view stats',
            type="number",
        ),
        html.Button('View Stats', id='node-input-button', n_clicks=0),
        html.Div(id='node-stats', children = [
            html.Table([
                # Header
                html.Tr([html.Th('Node Number'), html.Th('-', id='node-num')]),
                html.Tr([html.Td('No. Datapoints'), html.Td('-', id='node-data')]),
                html.Tr([html.Td('Mean Temperature'), html.Td('-', id='node-mean')]),
                html.Tr([html.Td('Minimum Temperature'), html.Td('-', id='node-min')]),
                html.Tr([html.Td('Maximum Temperature'), html.Td('-', id='node-max')]),
                html.Tr([html.Td('Temperature Variance'), html.Td('-', id='node-var')]),
            ]),
        ])
        ])],
        id='dash-container'
    )

    init_callbacks(dash_app)

    return dash_app.server

def init_callbacks(app):
    @app.callback(
        [Output('node-num', 'children'),
         Output('node-data', 'children'),
         Output('node-mean', 'children'),
         Output('node-min', 'children'),
         Output('node-max', 'children'),
         Output('node-var', 'children')],
        [Input('node-input-button', 'n_clicks')],
        [State('node-input', 'value')]        
    )
    def update_output(n_clicks, value):
        if n_clicks > 0:
            try:
                number = int(value)
            except ValueError:
                return ( 'Enter a number', '-', '-', '-', '-', '-' )
            except TypeError:
                return ( 'Enter valid character', '-', '-', '-', '-', '-' )

            df = pd.DataFrame(list(mongo.db.temp.find({'node': number})))
            if df.empty:
                return 'Node {} not found'.format(value), '-', '-', '-', '-', '-'
            
            return ('{}'.format(number),                  \
                    '{}'.format(df['temp'].count()),      \
                    '{:0.2f}'.format(df['temp'].mean()),  \
                    '{:0.2f}'.format(df['temp'].min()),   \
                    '{:0.2f}'.format(df['temp'].max()),   \
                    '{:0.2f}'.format(df['temp'].var()) )
        else:
            return ( '-', '-', '-', '-', '-', '-' )

    @app.callback(
        Output('temp-graph','figure'),
        [Input('refresh-graph', 'n_clicks')]
    )
    def update_figure(n_clicks):

        data = get_graph()

        return {
            'data': data,
            'layout': {
                'title': 'Temperature Graph',
                'height': 600,
                'padding': 150,
                'xaxis': {'title': {'text': 'Time of Reading (UTC)'}},
                'yaxis': {'title': {'text': 'Temperature (°C)'}},
            }
        }
